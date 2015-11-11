#pragma once

#include "mocca/base/ByteArray.h"
#include "mocca/base/Error.h"

#include <string>
#include <cstring>

namespace mocca {

class BytePacket {
public:
    BytePacket();
    BytePacket(ByteArray byteArray);

    const ByteArray& byteArray() const;

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    BytePacket& operator<<(T val) {
        byteArray_.append(&val, sizeof(T));
        return *this;
    }

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    BytePacket& operator>>(T& val) {
        if (readPos_ + sizeof(T) > byteArray_.size()) {
            throw Error("Reading beyond end of packet", __FILE__, __LINE__);
        }
        memcpy(&val, byteArray_.data() + readPos_, sizeof(T));
        readPos_ += sizeof(T);
        return *this;
    }

    BytePacket& operator<<(const std::string& val);
    BytePacket& operator>>(std::string& val);
    
    template<typename T> T get() {
        T val;
        *this >> val;
        return val;
    }

private:
    ByteArray byteArray_;
    size_t readPos_;
};
}