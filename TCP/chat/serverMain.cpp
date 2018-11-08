#include "Server.hpp"

namespace
{
int usage()
{
    std::cout << "Client <host> <port>\n";
    return 1;
}
} // namespace

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        return usage();
    }

    asio::io_context io;

    std::list<ChatServer> servers {};
    for (int i = 1; i < argc; ++i)
    {
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), std::atoi(argv[i]));
        servers.emplace_back(io, endpoint);
    }

    io.run();

    return 0;
}