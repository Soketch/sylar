#include <iostream>
#include "../sylar/config.h"
#include "../sylar/log.h"

sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup("system.port", (int)8080, "system port");

int main(int argc, char **argv)
{
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << g_int_value_config->getValue();
    SYLAR_LOG_INFO(SYLAR_lOG_ROOT()) << g_int_value_config->toString();
    return 0;
}