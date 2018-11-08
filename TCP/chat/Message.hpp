#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <array>
#include <algorithm>

class ChatMessage
{
  public:
    static constexpr uint8_t headerLength{4u};
    static constexpr uint16_t maxBodyLength{512u};

    ChatMessage()
        : mBodyLength{0u} {}

    char const *data() const
    {
        return mData.data();
    }

    char *data()
    {
        return mData.data();
    }

    std::size_t length() const
    {
        return mData.size();
    }

    char const *body() const
    {
        return mData.data() + headerLength;
    }

    char *body()
    {
        return mData.data() + headerLength;
    }

    size_t bodyLength() const
    {
        return mBodyLength;
    }

    void bodyLength(std::size_t newLen)
    {
        mBodyLength = std::min<size_t>(newLen, maxBodyLength);
    }

    bool decodeHeader()
    {
        char header[headerLength + 1] = "";
        std::strncat(header, mData.data(), headerLength);
        mBodyLength = std::atoi(header);
        if (mBodyLength > maxBodyLength)
        {
            mBodyLength = 0;
            return false;
        }
        return true;
    }

    void encodeHeader()
    {
        char header[headerLength + 1] = "";
        std::sprintf(header, "%4d", static_cast<int>(mBodyLength));
        std::copy_n(header, headerLength, mData.begin());
    }

  private:
    std::array<char, headerLength + maxBodyLength> mData{};
    std::size_t mBodyLength;
};