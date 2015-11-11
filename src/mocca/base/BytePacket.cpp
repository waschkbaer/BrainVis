#include "mocca/base/BytePacket.h"

namespace mocca {

    BytePacket::BytePacket() : byteArray_{}, readPos_(0) {}

BytePacket::BytePacket(ByteArray byteArray) : byteArray_(std::move(byteArray)), readPos_(0) {}

const ByteArray& BytePacket::byteArray() const { return byteArray_; }

BytePacket& BytePacket::operator<<(const std::string& val) {
    *this << val.size();
    byteArray_.append(val.c_str(), val.size());
    return *this;
}

BytePacket& BytePacket::operator>>(std::string& val) {
    std::string::size_type strSize;
    *this >> strSize;
    if (readPos_ + strSize > byteArray_.size()) {
        throw Error("Reading beyond end of packet", __FILE__, __LINE__);
    }
    val.reserve(strSize);
    val = std::string(byteArray_.data() + readPos_, strSize);
    readPos_ += strSize;
    return *this;
}
}