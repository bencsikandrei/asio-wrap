#include "Client.hpp"

namespace
{
int usage()
{
    std::cout << "Client <host> <port>\n";
    return 0;
}
} // namespace

int main(int argc, char const **argv)
{
    if (argc != 3)
    {
        return usage();
    }

    asio::io_service service{};

    asio::ip::tcp::resolver resolver{service};

    auto endpoints = resolver.resolve(argv[1], argv[2]);

    ChatClient client{service, endpoints};

    std::thread workerThread{
        [&service]() { service.run(); }};

    std::array<char, ChatMessage::maxBodyLength + 1> line{};
    while(std::cin.getline(line.data(), line.size())) {
        ChatMessage msg{};
        msg.bodyLength(std::strlen(line.data()));
        std::copy_n(line.begin(), msg.bodyLength(), msg.body());
        msg.encodeHeader();
        client.write(msg);
    }

    client.close();
    workerThread.join();

    return 0;
}