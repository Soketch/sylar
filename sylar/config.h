#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include "log.h"
namespace sylar
{
    // 配置基类  -- 用于配置基本信息（包含转换方法）
    class ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVarBase> ptr;

        ConfigVarBase(const std::string name, const std::string desc = "") : m_name(name), m_description(desc) {}
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

    template <class T>
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
                return boost::lexical_cast<std::string>(m_val);
            }
            catch (const std::exception &e)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception" << e.what()
                                                  << "  convert:" << typeid(m_val).name() << " to string"
                                                  << '\n';
            }
            return "";
        }
        bool fromString(const std::string &val) override
        {
            try
            {
                m_val = boost::lexical_cast<T>(val);
            }
            catch (const std::exception &e)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString exception" << e.what()
                                                  << "  convert:" << typeid(m_val).name() << " from string"
                                                  << '\n';
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
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._012345678") != std::string::npos)
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

    private:
        static ConfigVarMap s_datas;
    };

}

#endif