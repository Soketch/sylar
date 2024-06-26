#include <iostream>
#include "../sylar/config.h"
#include "../sylar/log.h"
#include "yaml-cpp/yaml.h"

#define YAML_TEST_FILE "/home/coding/cpp/sylar/bin/conf/test.yml"

#if 0
sylar::ConfigVar<int>::ptr g_int_value_config =
    sylar::Config::Lookup("system.port", (int)8080, "system port");

// 测试问题
sylar::ConfigVar<float>::ptr g_int_valuex_config =
    sylar::Config::Lookup("system.port", (float)8080, "system port");

sylar::ConfigVar<float>::ptr g_float_value_config =
    sylar::Config::Lookup("system.value", (float)10.82, "system value");

// 测试复杂类型 vector<int>
sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config =
    sylar::Config::Lookup("system.int_vec", std::vector<int>{100, 200}, "system int_vector");

sylar::ConfigVar<std::list<int>>::ptr g_int_list_value_config =
    sylar::Config::Lookup("system.int_list", std::list<int>{10, 20}, "system int list val");

sylar::ConfigVar<std::set<int>>::ptr g_int_set_value_config =
    sylar::Config::Lookup("system.int_set", std::set<int>{10, 20}, "system int set val");

sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config =
    sylar::Config::Lookup("system.int_uset", std::unordered_set<int>{10, 20}, "system int unordered_set val");

sylar::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config =
    sylar::Config::Lookup("system.str_int_map", std::map<std::string, int>{{"key1", 1}, {"key2", 2}}, "system string int map val");

sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config =
    sylar::Config::Lookup("system.str_int_umap", std::unordered_map<std::string, int>{{"key1", 1}, {"key2", 2}}, "system string int unordered_map val");

void print_yaml(const YAML::Node &node, int level)
{
    if (node.IsScalar())
    {
        SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << std::string(level * 4, ' ')
                                         << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if (node.IsNull())
    {
        SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << std::string(level * 4, ' ')
                                         << "Null - " << node.Type() << " - " << level;
    }
    else if (node.IsMap())
    {
        for (auto it = node.begin(); it != node.end(); ++it)
        {
            SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << std::string(level * 4, ' ')
                                             << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }
    else if (node.IsSequence())
    {
        for (size_t i = 0; i < node.size(); ++i)
        {
            SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << std::string(level * 4, ' ')
                                             << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

// yaml 测试
void test_yaml()
{
    YAML::Node root = YAML::LoadFile(YAML_TEST_FILE);
    print_yaml(root, 0);
}

void test_sylar1()
{
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << g_int_value_config->toString();
    std::cout << " ------------------------" << std::endl;

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_value_config->toString();
    std::cout << " ------------------------" << std::endl;
}

void test_config()
{
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << "before: " << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << "before: " << g_float_value_config->toString();

#define XX(g_var, name, prefix)                                                               \
    {                                                                                         \
        auto &v = g_var->getValue();                                                          \
        for (auto &i : v)                                                                     \
        {                                                                                     \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": " << i;                  \
        }                                                                                     \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

// map的特殊遍历宏
#define XX_M(g_var, name, prefix)                                                             \
    {                                                                                         \
        auto &v = g_var->getValue();                                                          \
        for (auto &i : v)                                                                     \
        {                                                                                     \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": {"                       \
                                             << i.first << " - " << i.second << "}";          \
        }                                                                                     \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
    }

    XX(g_int_vec_value_config, int_vec, before);
    XX(g_int_list_value_config, int_list, before);
    XX(g_int_set_value_config, int_set, before);
    XX(g_int_uset_value_config, int_uset, before);
    XX_M(g_str_int_map_value_config, str_int_map, before);
    XX_M(g_str_int_umap_value_config, str_int_umap, before);

    YAML::Node root = YAML::LoadFile("/home/coding/cpp/sylar/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << "after: " << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_value_config, int_vec, after);
    XX(g_int_list_value_config, int_list, after);
    XX(g_int_set_value_config, int_set, after);
    XX(g_int_uset_value_config, int_uset, after);
    XX_M(g_str_int_map_value_config, str_int_map, after);
    XX_M(g_str_int_umap_value_config, str_int_umap, after);
}
#endif

class Person
{
public:
    Person() {}
    ~Person() {}
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const
    {
        std::stringstream ss;
        ss << "[Person name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << "]";
        return ss.str();
    }

    bool operator==(const Person &oth) const
    {
        return m_name == oth.m_name &&
               m_age == oth.m_age &&
               m_sex == oth.m_sex;
    }
};

namespace sylar
{
    template <>
    class LexicalCast<std::string, Person>
    {
    public:
        Person operator()(const std::string &v)
        {
            YAML::Node node = YAML::Load(v);
            Person p;
            std::stringstream ss;
            p.m_name = node["name"].as<std::string>();
            p.m_age = node["age"].as<int>();
            p.m_sex = node["sex"].as<bool>();
            return p;
        }
    };
    template <>
    class LexicalCast<Person, std::string>
    {
    public:
        std::string operator()(const Person &p)
        {
            YAML::Node node;
            node["name"] = p.m_name;
            node["age"] = p.m_age;
            node["sex"] = p.m_sex;

            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };
}

sylar::ConfigVar<Person>::ptr g_person =
    sylar::Config::Lookup("class.person", Person(), "system person");

// 自定义类型结合stl
sylar::ConfigVar<std::map<std::string, Person>>::ptr g_person_map =
    sylar::Config::Lookup("class.map", std::map<std::string, Person>(), "system map person");

// 自定义类型配置支持
void test_class()
{

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before: " << g_person->getValue().toString() << " - " << g_person->toString();

#define XX_PM(g_var, prefix)                                                   \
    {                                                                          \
        auto m = g_var->getValue();                                            \
        for (auto &i : m)                                                      \
        {                                                                      \
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix ": " << i.first        \
                                             << " - " << i.second.toString();  \
        }                                                                      \
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << prefix << ": size = " << m.size(); \
    }

    g_person->addListener([](const Person &old_val, const Person &new_val)
                          { SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << " old_val=" << old_val.toString()
                                                             << " new_val=" << new_val.toString(); });

    XX_PM(g_person_map, "class.map before");

    YAML::Node root = YAML::LoadFile("/home/coding/cpp/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after: " << g_person->getValue().toString() << " - " << g_person->toString();

    XX_PM(g_person_map, "class.map after");
}

void test_log()
{
    static sylar::Logger::ptr system_log = SYLAR_LOG_NAME("system");
    SYLAR_LOG_INFO(system_log) << "hello, system" << std::endl;

    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << std::endl;

    YAML::Node root = YAML::LoadFile("/home/coding/cpp/sylar/bin/conf/test.yml");
    sylar::Config::LoadFromYaml(root);

    std::cout << "=====================" << std::endl;
    std::cout << sylar::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    std::cout << "=====================" << std::endl;

    std::cout << root << std::endl;
    std::cout << "********************" << std::endl;

    SYLAR_LOG_INFO(system_log) << "hello, system" << std::endl;

    system_log->setFormatter("%d - %m%n");
    SYLAR_LOG_INFO(system_log) << "hello, system" << std::endl;
}

int main(int argc, char **argv)
{
    // test_sylar1();
    // test_yaml();
    // test_config();
    // test_class();

    std::cout << "--project run start--" << std::endl;
    test_log();
    // sylar::Config::Visit(
    //     [](sylar::ConfigVarBase::ptr var)
    //     {
    //         SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "name=" << var->getName()
    //                                          << " description=" << var->getDescription()
    //                                          << " typename=" << var->getTypeName()
    //                                          << " value=" << var->toString();
    //     });
    std::cout << "--project run end--" << std::endl;
    return 0;
}