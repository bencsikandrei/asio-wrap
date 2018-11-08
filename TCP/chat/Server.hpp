#pragma once

#include "Message.hpp"

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"

using ChatMessageQueue_t = std::deque<ChatMessage>;

class ChatParticipant
{
  public:
    virtual ~ChatParticipant() = default;
    virtual void deliver(ChatMessage const &msg) = 0;
};

using ChatParticipantPtr_t = std::shared_ptr<ChatParticipant>;

class ChatRoom
{
  public:
    void join(ChatParticipantPtr_t participant)
    {
        mParticipants.insert(participant);
        for (auto const &msg : mRecentMessages)
        {
            participant->deliver(msg);
        }
    }
    void leave(ChatParticipantPtr_t participant)
    {
        mParticipants.erase(participant);
    }
    void deliver(ChatMessage const &msg)
    {
        mRecentMessages.push_back(msg);
        while (mRecentMessages.size() > maxRecentMessages)
        {
            mRecentMessages.pop_front();
        }
        for (auto &participant : mParticipants)
        {
            participant->deliver(msg);
        }
    }

  private:
    std::set<ChatParticipantPtr_t> mParticipants{};
    static constexpr uint8_t maxRecentMessages{100u};
    ChatMessageQueue_t mRecentMessages{};
};

class ChatSession
    : public ChatParticipant,
      public std::enable_shared_from_this<ChatSession>
{
  public:
    ChatSession(asio::ip::tcp::socket socket, ChatRoom &room)
        : mSocket(std::move(socket)),
          mRoom(room)
    {
    }

    void start()
    {
        mRoom.join(shared_from_this());
        doReadHeader();
    }

    void deliver(const ChatMessage &msg)
    {
        bool writeInProgress = not mWriteMessages.empty();
        mWriteMessages.push_back(msg);
        if (writeInProgress)
            return;
        doWrite();
    }

  private:
    void doReadHeader()
    {
        auto self(shared_from_this());
        asio::async_read(mSocket,
                         asio::buffer(mReadMessage.data(), ChatMessage::headerLength),
                         [this, self](std::error_code ec, std::size_t /*length*/) {
                             if (!ec && mReadMessage.decodeHeader())
                             {
                                 doReadBody();
                             }
                             else
                             {
                                 mRoom.leave(shared_from_this());
                             }
                         });
    }

    void doReadBody()
    {
        auto self(shared_from_this());
        asio::async_read(mSocket,
                         asio::buffer(mReadMessage.body(), mReadMessage.bodyLength()),
                         [this, self](std::error_code ec, std::size_t /*length*/) {
                             if (!ec)
                             {
                                 mRoom.deliver(mReadMessage);
                                 doReadHeader();
                             }
                             else
                             {
                                 mRoom.leave(shared_from_this());
                             }
                         });
    }

    void doWrite()
    {
        auto self(shared_from_this());
        asio::async_write(mSocket,
                          asio::buffer(mWriteMessages.front().data(),
                                       mWriteMessages.front().length()),
                          [this, self](std::error_code ec, std::size_t /*length*/) {
                              if (!ec)
                              {
                                  mWriteMessages.pop_front();
                                  if (!mWriteMessages.empty())
                                  {
                                      doWrite();
                                  }
                              }
                              else
                              {
                                  mRoom.leave(shared_from_this());
                              }
                          });
    }

    asio::ip::tcp::socket mSocket;
    ChatRoom &mRoom;
    ChatMessage mReadMessage;
    ChatMessageQueue_t mWriteMessages;
};

class ChatServer
{
  public:
    ChatServer(asio::io_context &io,
               const asio::ip::tcp::endpoint &endpoint)
        : mAcceptor(io, endpoint)
    {
        doAccept();
    }

  private:
    void doAccept()
    {
        mAcceptor.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec)
                {
                    std::make_shared<ChatSession>(std::move(socket), mRoom)->start();
                }

                doAccept();
            });
    }

    asio::ip::tcp::acceptor mAcceptor;
    ChatRoom mRoom;
};
