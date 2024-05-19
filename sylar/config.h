#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <yaml-cpp/yaml.h>
#include "log.h"

namespace sylar
{
    // 配置基类  -- 用于配置基本信息（包含转换方法）
    class ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        ConfigVarBase(const std::string name, const std::string desc = "") : m_name(name), m_description(desc)
        {
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }
        virtual ~ConfigVarBase() {}
        const std::string getName() const { return m_name; }
        const std::string getDescription() const { return m_description; }

        virtual std::string toString() = 0; // 转换为明文
        virtual bool fromString(const std::string &val) = 0;

    protected:
        std::string m_name;
        std::string m_description; // 描述

    private:
    };

    // F from_type, T to_type
    template <class F, class T>
    class LexicalCast
    {
    public:
        T operator()(const F &v)
        {
            return boost::lexical_cast<T>(v);
        }
    };
    // LexicalCast的偏特化
    template <class T> // 将string转vector
    class LexicalCast<std::string, std::vector<T>>
    {
    public:
        std::vector<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::vector<T> vec; // 定义返回类型
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec; // return std::move(vec);
        }
    };
    template <class T> // 将vector转string
    class LexicalCast<std::vector<T>, std::string>
    {
    public:
        std::string operator()(const std::vector<T> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    // 利用仿函数
    // FromStr T operator() (const std::string&)   // 将 string 类型 可以转换为 自定义类型 T
    // ToStr std::string operator() (const T&)     // 自定义类型 T 转换为 string 类型
    template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
    class ConfigVar : public ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVar> ptr;

        ConfigVar(const std::string &name, const T &default_value, const std::string description = "")
            : ConfigVarBase(name, description), m_val(default_value)
        {
        }

        std::string toString() override
        {
            try
            {
                // return boost::lexical_cast<std::string>(m_val);
                return ToStr()(m_val);
            }
            catch (const std::exception &e)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception" << e.what()
                                                  << "  convert:" << typeid(m_val).name() << " to string";
            }
            return "";
        }
        bool fromString(const std::string &val) override
        {
            try
            {
                // m_val = boost::lexical_cast<T>(val);
                setValue(FromStr()(val));
            }
            catch (const std::exception &e)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception" << e.what()
                                                  << "  convert:" << typeid(m_val).name() << " from string";
            }
            return false;
        }
        const T getValue() const { return m_val; }
        void setValue(const T &value) { m_val = value; }

    private:
        T m_val;
    };

    // config  管理类
    class Config
    {
    public:
        typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name, const T &default_value, const std::string desc = "")
        {
            auto tmp = Lookup<T>(name);
            if (tmp)
            {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "==config==  Lookup name:" << name << " exists";
                return tmp;
            }
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "==config==  Lookup name Invalid" << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, desc));
            s_datas[name] = v;
            return v;
        }

        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name)
        {
            auto it = s_datas.find(name);
            if (it == s_datas.end())
            {
                return nullptr;
            }
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }

        static void LoadFromYaml(const YAML::Node &root);
        static ConfigVarBase::ptr LookupBase(const std::string &name);

    private:
        static ConfigVarMap s_datas;
    };

}

#endif