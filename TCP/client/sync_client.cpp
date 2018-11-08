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
    explicit TcpClient()
        : mService{},
          mSocket{mService}
    {
    }
    void connect(std::string ipAddr, uint16_t port)
    {
        mSocket.connect({asio::ip::address::from_string(ipAddr), port});
    }
    template <typename Buffer>
    size_t write(Buffer &&buf)
    {
        return mSocket.write_some(std::forward<Buffer>(buf));
    }
    template <typename Buffer>
    size_t read(Buffer &&buf)
    {
        return mSocket.read_some(std::forward<Buffer>(buf));
    }
    void shutdown() {
        mSocket.shutdown(asio::ip::tcp::socket::shutdown_receive);
    }
    void close() {
        mSocket.close();
    }
    size_t available() const {
        return mSocket.available();
    }

  private:
    asio::io_service mService;
    asio::ip::tcp::socket mSocket;
};
} // namespace synch

int main()
{
    synch::TcpClient tcpClient{};
    char buf[1024];
    try
    {
        tcpClient.connect("127.0.0.1", 2001);
        tcpClient.write(asio::buffer("Hello!"));
        std::clog << "Bytes available for synch read: " << tcpClient.available() << '\n';
        size_t readBytes = tcpClient.read(asio::buffer(buf, 1024));
        std::clog << "Data received: " << buf << ", size: " << readBytes << '\n';
        tcpClient.shutdown();
        tcpClient.close();
    }
    catch (std::exception const &e)
    {
        std::cout << e.what() << '\n';
    }
}