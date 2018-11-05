#include <asio.hpp>
#include <memory>
#include <thread>
#include <iostream>

using asio_tcp_socket = asio::ip::tcp::socket;
using socket_ptr = std::shared_ptr<asio_tcp_socket>;

namespace synch
{
class TcpClient
{
  public:
    explicit TcpClient(std::string ipAddr, uint16_t port)
        : mService{}, mEndPoint{
                          asio::ip::address::from_string(ipAddr), port},
          mSocket{mService}
    {
    }
    void connect()
    {
        mSocket.connect(mEndPoint);
    }

  private:
    asio::io_service mService;
    asio::ip::tcp::endpoint mEndPoint;
    asio::ip::tcp::socket mSocket;
};
} // namespace synch

int main()
{
    synch::TcpClient tcpClient{"127.0.0.1", 2001};
    try
    {
        tcpClient.connect();
    }
    catch (std::exception const &e)
    {
        std::cout << e.what() << '\n';
    }
}