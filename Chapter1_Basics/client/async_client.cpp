#include <asio.hpp>
#include <memory>
#include <thread>
#include <iostream>
using asio_tcp_socket = asio::ip::tcp::socket;
using socket_ptr = std::shared_ptr<asio_tcp_socket>;

namespace async
{
class TcpClient
{
    using asio_tcp_socket = asio::ip::tcp::socket;
    using socket_ptr = std::shared_ptr<asio_tcp_socket>;

  public:
    explicit TcpClient(uint16_t port, uint8_t ipVersion = 4)
        : mService{}, mEndPoint{
                          (ipVersion == 4) ? asio::ip::tcp::v4() : asio::ip::tcp::v6(), port},
          mSocket{mService}
    {
    }
    explicit TcpClient(std::string ipAddr, uint16_t port)
        : mService{}, mEndPoint{
                          asio::ip::address::from_string(ipAddr), port},
          mSocket{mService}
    {
    }
    void run()
    {
        mService.run();
    }
    template <typename Handler>
    void connect(Handler &&handler)
    {
        mSocket.async_connect(mEndPoint, std::forward<Handler>(handler));
    }

  private:
    asio::io_service mService{};
    asio::ip::tcp::endpoint mEndPoint;
    asio::ip::tcp::socket mSocket;
};
} // namespace async

void connectHandler(std::system_error const &ec)
{
    if (ec.code())
    {
        std::cout << "This happened: " << ec.code() << ", meaning " << ec.what() << "\n";
        return;
    }
    std::cout << "Connection succesful\n";
}

int main()
{
    async::TcpClient client{"127.0.0.1", 2001};
    client.connect([](std::system_error const& ec) {
        connectHandler(ec);
    });
    client.run();
}