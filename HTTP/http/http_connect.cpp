#include <asio.hpp>
#include <memory>
#include <thread>
#include <iostream>
#include <system_error>

using asio_tcp_socket = asio::ip::tcp::socket;
using socket_ptr = std::shared_ptr<asio_tcp_socket>;


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
    asio::io_service service;
    asio::ip::tcp::resolver resolver {service};
    asio::ip::tcp::resolver::query query {"www.yahoo.com", "80"};
    auto endPoints = resolver.resolve(query);
    asio::ip::tcp::endpoint ep { *endPoints };
    std::cout << ep.address().to_string() << ':' << ep.port() << '\n';
}