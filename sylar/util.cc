#include "util.h"
#include "log.h"
#include <execinfo.h>

namespace sylar
{
    sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

    pid_t GetTheadId()
    {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberId()
    {
        return 0;
    }

    void BackTrace(std::vector<std::string> &bt, int size, int skip)
    {
        void **array = (void **)malloc((sizeof(void *) * size));
        size_t s = ::backtrace(array, size);

        char **strings = backtrace_symbols(array, s);
        if (strings == NULL)
        {
            SYLAR_LOG_ERROR(g_logger) << "backtrace_symbols error";
            return;
        }
        for (size_t i = skip; i < s; ++i) // 跳过前 skip 个堆栈帧，将剩余的帧添加到 bt 向量中。
        {
            bt.push_back(strings[i]);
        }

        free(strings);
        free(array);
    }

    std::string BackTraceToString(int size, int skip, const std::string &prefix)
    {
        std::vector<std::string> bt;
        BackTrace(bt, size, skip);
        std::stringstream ss;
        for (size_t i = 0; i < bt.size(); ++i)
        {
            ss << prefix << bt[i] << std::endl;
        }
        return ss.str();
    }
}