#include "sylar/tcp_server.h"
#include "sylar/log.h"
#include "sylar/iomanager.h"
#include "sylar/bytearray.h"
#include "sylar/address.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
int g_type = 1;

class EchoServer : public sylar::TcpServer
{
public:
    EchoServer(int type);
    void handleClient(sylar::Socket::ptr client);

private:
    int m_type = 0;
};

EchoServer::EchoServer(int type)
    : m_type(type)
{
}
void EchoServer::handleClient(sylar::Socket::ptr client)
{
    SYLAR_LOG_INFO(g_logger) << "handleClient " << *client;
    sylar::ByteArray::ptr ba(new sylar::ByteArray);
    while (true)
    {
        ba->clear();
        std::vector<iovec> iovs;

        ba->getWriteBuffers(iovs, 1024);

        int ret = client->recv(&iovs[0], iovs.size());
        if (ret == 0)
        {
            SYLAR_LOG_INFO(g_logger) << "client close:" << *client;
            break;
        }
        else if (ret < 0)
        {
            SYLAR_LOG_ERROR(g_logger) << "client error, ret=" << ret
                                      << ",  errno=" << errno
                                      << ",  str(errno):" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + ret);
        ba->setPosition(0);
        // SYLAR_LOG_INFO(g_logger) << "m_type" << this->m_type << ", g_type" << g_type;
        if (m_type == 1)
        { // -t  test
            SYLAR_LOG_INFO(g_logger) << ba->toString();
        }
        else
        { // -b bianry  0x
            SYLAR_LOG_INFO(g_logger) << ba->toHexString();
        }
    }
}

void run()
{
    EchoServer::ptr es(new EchoServer(g_type));
    auto addr = sylar::Address::LookupAny("0.0.0.0:8020");

    while (!es->bind(addr))
    {
        sleep(2);
    }
    es->start();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        SYLAR_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }
    if (!strcmp(argv[1], "-b"))
    {
        g_type = 2;
    }

    // SYLAR_LOG_INFO(g_logger) << "g_type=" << g_type << ",  argv=" << argv[1];
    sylar::IOManager iom(2);
    iom.schedule(run);

    return 0;
}