#include "mocca/base/ByteArray.h"
#include "mocca/base/Error.h"

#include <cstring>

namespace mocca {

ByteArray::ByteArray(size_t capacity) : data_(new char[capacity]), capacity_(capacity), size_(0) {}

ByteArray::ByteArray(const ByteArray& other)
    : data_(new char[other.capacity_]), capacity_(other.capacity_), size_(other.size_) {
    memcpy(data_.get(), other.data_.get(), other.size_);
}

mocca::ByteArray::ByteArray(ByteArray&& other)
    : data_(std::move(other.data_)), capacity_(other.capacity_), size_(other.size_) {
    other.capacity_ = 0;
    other.size_ = 0;
}

ByteArray& mocca::ByteArray::operator=(ByteArray other) {
    ByteArray tmp(std::move(other));
    std::swap(tmp, *this);
    return *this;
}

char* ByteArray::data() { return data_.get(); }

const char* ByteArray::data() const { return data_.get(); }

size_t ByteArray::size() const { return size_; }

void ByteArray::setSize(size_t size) {
    if (size > capacity_) {
        throw Error("Size exceeds capacity of the byte array", __FILE__, __LINE__);
    }
    size_ = size;
}

bool ByteArray::isEmpty() const { return size_ == 0; }

size_t ByteArray::capacity() const { return capacity_; }

void ByteArray::resize(size_t newCapacity) {
    auto newData = std::unique_ptr<char[]>(new char[newCapacity]);
    memcpy(newData.get(), data_.get(), size_);
    data_ = std::move(newData);
    capacity_ = newCapacity;
}

void ByteArray::append(const void* data, size_t size) {
    if (capacity_ < size_ + size) {
        resize(size_ + 2 * size);
    }
    memcpy(data_.get() + size_, data, size);
    size_ += size;
}

ByteArray ByteArray::createFromRaw(const void* raw, size_t size) {
    auto byteArray = ByteArray(size);
    memcpy(byteArray.data_.get(), raw, size);
    byteArray.size_ = size;
    return byteArray;
}
}