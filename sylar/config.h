#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <memory>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>
#include <yaml-cpp/yaml.h>
#include <functional>
#include <vector> //支持更多容器的偏特化
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include "log.h"
#include "thread.h"

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
        virtual std::string getTypeName() const = 0;

    protected:
        std::string m_name;
        std::string m_description; // 描述

    private:
    };

    // LexicalCast的偏特化
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
    // vector的偏特化
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
    // list偏特化
    template <class T> // 将string转list
    class LexicalCast<std::string, std::list<T>>
    {
    public:
        std::list<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::list<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };
    template <class T> // 将list转string
    class LexicalCast<std::list<T>, std::string>
    {
    public:
        std::string operator()(const std::list<T> &v)
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
    // set偏特化
    template <class T> // 将string转set
    class LexicalCast<std::string, std::set<T>>
    {
    public:
        std::set<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::set<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };
    template <class T> // 将set转string
    class LexicalCast<std::set<T>, std::string>
    {
    public:
        std::string operator()(const std::set<T> &v)
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
    // unordered_set偏特化
    template <class T> // 将string转unordered_set
    class LexicalCast<std::string, std::unordered_set<T>>
    {
    public:
        std::unordered_set<T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_set<T> vec;
            std::stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<std::string, T>()(ss.str()));
            }
            return vec;
        }
    };
    template <class T> // 将unordered_set转string
    class LexicalCast<std::unordered_set<T>, std::string>
    {
    public:
        std::string operator()(const std::unordered_set<T> &v)
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
    // map偏特化   key => string      value => T
    template <class T> // 将string转map
    class LexicalCast<std::string, std::map<std::string, T>>
    {
    public:
        std::map<std::string, T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::map<std::string, T> vec;
            std::stringstream ss;
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                                          LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };
    template <class T> // 将map转string
    class LexicalCast<std::map<std::string, T>, std::string>
    {
    public:
        std::string operator()(const std::map<std::string, T> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };
    // unordered_map偏特化   key => string      value => T
    template <class T> // 将string转unorderedd_map
    class LexicalCast<std::string, std::unordered_map<std::string, T>>
    {
    public:
        std::unordered_map<std::string, T> operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            typename std::unordered_map<std::string, T> vec;
            std::stringstream ss;
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ss.str("");
                ss << it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                                          LexicalCast<std::string, T>()(ss.str())));
            }
            return vec;
        }
    };
    template <class T> // 将unordered_map转string
    class LexicalCast<std::unordered_map<std::string, T>, std::string>
    {
    public:
        std::string operator()(const std::unordered_map<std::string, T> &v)
        {
            YAML::Node node;
            for (auto &i : v)
            {
                node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
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
        typedef RWMutex RWMutexType; // 读写锁
        typedef std::shared_ptr<ConfigVar>
            ptr;
        typedef std::function<void(const T &old_val, const T &new_val)> on_change_cb; // 回调函数

        ConfigVar(const std::string &name, const T &default_value, const std::string description = "")
            : ConfigVarBase(name, description), m_val(default_value)
        {
        }

        std::string toString() override
        {
            try
            {
                // return boost::lexical_cast<std::string>(m_val);
                RWMutexType::ReadLock lock(m_mutex);
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
                                                  << "  convert: string to " << typeid(m_val).name() << " - "
                                                  << val;
            }
            return false;
        }
        const T getValue() const
        {
            RWMutexType::ReadLock lock(m_mutex);
            return m_val;
        }
        void setValue(const T &v) // 这里有两个操作，前面是读->用读锁，后面是写->用写锁
        {
            {
                RWMutexType::ReadLock lock(m_mutex); // 作用域内是读操作->用读锁
                if (v == m_val)
                { // 如果原值等于新值， 没有变化
                    return;
                }

                // 不是，变化了，进行回调  --> 写操作
                for (auto &i : m_cbs)
                {
                    i.second(m_val, v); // 这里 i 是m_cbs数组中的回调函数[key, callback]， //通知回调函数当前值和新值
                }
            }
            RWMutexType::WriteLock lock(m_mutex);
            m_val = v; // 更新值  -->写操作
        }
        std::string getTypeName() const override { return typeid(T).name(); }

        // 对回调函数 - 增加监听
        uint64_t addListener(on_change_cb cb)
        {
            static uint64_t s_fun_id = 0; // key => 静态局部变量自动生成
            RWMutexType::WriteLock lock(m_mutex);
            ++s_fun_id;
            m_cbs[s_fun_id] = cb;
            return s_fun_id;
        }
        // 删除
        void delListener(uint64_t key)
        {
            RWMutexType::WriteLock lock(m_mutex);
            m_cbs.erase(key);
        }
        // 返回cb
        on_change_cb getListener(u_int64_t key)
        {
            RWMutexType::ReadLock lock(m_mutex);
            auto it = m_cbs.find(key);
            return it == m_cbs.end() ? nullptr : it->second;
        }

        void clearListener()
        {
            RWMutexType::WriteLock lock(m_mutex);
            m_cbs.clear();
        }

    private:
        mutable RWMutexType m_mutex; // 读写锁互斥量
        T m_val;

        // 变更回调数组，uint64_t key要求唯一，一般使用hash
        std::map<uint64_t, on_change_cb> m_cbs; // 采用map是因为functional中没有比较函数，意味着无法判断是否是相同回调函数
    };

    // config  管理类
    class Config
    {
    public:
        typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
        typedef RWMutex RWMutexType; // 读写锁

        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name, const T &default_value, const std::string desc = "")
        {
            RWMutexType::WriteLock lock(GetMutex());
            // auto it = s_datas.find(name);
            auto it = GetDtatas().find(name);
            if (it != GetDtatas().end())
            {
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
                if (tmp)
                {
                    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "==config==  Lookup name:" << name << " exists";
                    return tmp;
                }
                else
                {
                    SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "==config==  Lookup name:" << name << " exists but type not"
                                                      << typeid(T).name() << " real_type=" << it->second->getTypeName()
                                                      << " " << it->second->toString();
                    return nullptr;
                }
            }

            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
            {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "==config==  Lookup name Invalid" << name;
                throw std::invalid_argument(name);
            }

            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, desc));
            GetDtatas()[name] = v;
            return v;
        }

        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const std::string &name)
        {
            RWMutexType::ReadLock lock(GetMutex());
            auto it = GetDtatas().find(name);
            if (it == GetDtatas().end())
            {
                return nullptr;
            }
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }

        /**
         * @brief 使用YAML::Node初始化配置模块
         */
        static void LoadFromYaml(const YAML::Node &root);

        /**
         * @brief 加载path文件夹里面的配置文件
         */
        static void LoadFromConfDir(const std::string &path, bool force = false);

        /**
         * @brief 查找配置参数,返回配置参数的基类
         * @param[in] name 配置参数名称
         */
        static ConfigVarBase::ptr LookupBase(const std::string &name);

        /**
         * @brief 遍历配置模块里面所有配置项
         * @param[in] cb 配置项回调函数
         */
        static void Visit(std::function<void(ConfigVarBase::ptr)> cb);

    private:
        // 返回所有的配置项
        static ConfigVarMap &GetDtatas()
        {
            static ConfigVarMap s_datas;
            return s_datas;
        }

        // 配置项的RWMutex
        static RWMutexType &GetMutex()
        {
            static RWMutexType s_mutex;
            return s_mutex;
        }
    };

}

#endif