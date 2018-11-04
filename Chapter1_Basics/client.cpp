#include <asio.hpp>
#include <memory>
#include <thread>

using asio_tcp_socket = asio::ip::tcp::socket;
using socket_ptr = std::shared_ptr<asio_tcp_socket>;

void clientSession(socket_ptr sock)
{
    while (true)
    {
        char data[512];
        size_t len = sock->read_some(asio::buffer(data));
        if (len > 0)
        {
            write(*sock, asio::buffer("ok\n", 3));
        }
    }
}

class TcpClient
{
    using asio_tcp_socket = asio::ip::tcp::socket;
    using socket_ptr = std::shared_ptr<asio_tcp_socket>;

  public:
    explicit TcpClient(uint16_t port, uint8_t ipVersion = 4)
        : mService{}, mEndPoint{
                          (ipVersion == 4) ? asio::ip::tcp::v4() : asio::ip::tcp::v6(), port},
          mAcceptor{mService, mEndPoint}
    {
    }
    template <typename Function>
    void listen(Function &&func)
    {
        while (true)
        {
            socket_ptr sock{std::make_shared<asio_tcp_socket>(mService)};
            mAcceptor.accept(*sock);

            std::thread(std::forward<Function>(func), sock).detach();
        }
    }

  private:
    asio::io_service mService{};
    asio::ip::tcp::endpoint mEndPoint;
    asio::ip::tcp::acceptor mAcceptor;
};

int main()
{
    TcpClient tcpClient{2001};
    tcpClient.listen(
        [](socket_ptr sock) {
            clientSession(sock);
        });
}