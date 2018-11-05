#include <asio.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include <system_error>

using asio_tcp_socket = asio::ip::tcp::socket;
using socket_ptr = std::shared_ptr<asio_tcp_socket>;

void clientSession(socket_ptr sock)
{
    std::error_code error;
    while (true)
    {
        char data[512];
        size_t len = sock->read_some(asio::buffer(data), error);
        if (error == asio::error::eof)
        {
            std::cout << "Session ended with EOF.\n";
            break;
        }
        if (len <= 0)
            continue;
        write(*sock, asio::buffer("ok\n", 3));
    }
}

namespace synch
{
class TcpServer
{
    using asio_tcp_socket = asio::ip::tcp::socket;
    using socket_ptr = std::shared_ptr<asio_tcp_socket>;

  public:
    explicit TcpServer(uint16_t port, uint8_t ipVersion = 4)
        : mService{}, mEndPoint{
                          (ipVersion == 4) ? asio::ip::tcp::v4() : asio::ip::tcp::v6(), port},
          mAcceptor{mService, mEndPoint}
    {
    }
    template <typename Handler>
    void listen(Handler &&handler)
    {
        while (true)
        {
            socket_ptr sock{std::make_shared<asio_tcp_socket>(mService)};
            mAcceptor.accept(*sock);

            std::thread(std::forward<Handler>(handler), sock).detach();
        }
    }

  private:
    asio::io_service mService{};
    asio::ip::tcp::endpoint mEndPoint;
    asio::ip::tcp::acceptor mAcceptor;
};
} // namespace synch

int main()
{
    synch::TcpServer server{2001};
    server.listen(clientSession);
}