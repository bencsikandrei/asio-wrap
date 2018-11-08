#include "Message.hpp"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <asio.hpp>

using ChatMessageQueue_t = std::deque<ChatMessage>;

class ChatClient
{
  public:
    ChatClient(asio::io_service &io,
               asio::ip::tcp::resolver::results_type const &endpoints) : mIo{io},
                                                                         mSocket{io}
    {
        doConnect(endpoints);
    }

    void write(ChatMessage const &msg)
    {
        asio::post(mIo,
                   [this, msg]() {
                       bool writeInProgress{not mWriteMessages.empty()};
                       if (writeInProgress)
                           return;
                       doWrite();
                   });
    }

    void close()
    {
        asio::post(mIo, [this]() {
            mSocket.close();
        });
    }

  private:
    void doConnect(asio::ip::tcp::resolver::results_type const &endpoints)
    {
        asio::async_connect(mSocket, endpoints,
                            [this](std::error_code ec, asio::ip::tcp::endpoint) {
                                if (ec)
                                {
                                    std::cout << "Could not connect!\nError: " << ec << '\n';
                                    return;
                                }
                                doReadHeader();
                            });
    }
    void doReadHeader()
    {
        asio::async_read(mSocket,
                         asio::buffer(mReadMessage.data(), ChatMessage::headerLength),
                         [this](std::error_code ec, std::size_t length) {
                             if (ec or not mReadMessage.decodeHeader())
                             {
                                 std::cout << "Error: " << ec << '\n';
                                 mSocket.close();
                                 return;
                             }
                             doReadBody();
                         });
    }
    void doReadBody()
    {
        asio::async_read(mSocket,
                         asio::buffer(mReadMessage.body(), mReadMessage.bodyLength()),
                         [this](std::error_code ec, std::size_t length) {
                             if (ec)
                             {
                                 mSocket.close();
                                 return;
                             }

                             std::cout.write(mReadMessage.body(), mReadMessage.bodyLength());
                             std::cout << '\n';
                             doReadHeader();
                         });
    }
    void doWrite()
    {
        asio::async_write(mSocket,
                          asio::buffer(mWriteMessages.front().data(), mWriteMessages.front().length()),
                          [this](std::error_code ec, std::size_t length) {
                              if (ec)
                              {
                                  mSocket.close();
                                  return;
                              }
                              mWriteMessages.pop_front();
                              if (mWriteMessages.empty())
                                  return;
                              doWrite();
                          });
    }

  private:
    asio::io_service &mIo;
    asio::ip::tcp::socket mSocket;
    ChatMessage mReadMessage{};
    ChatMessageQueue_t mWriteMessages{};
};
