#pragma once

#include <memory>

namespace mocca {

class ByteArray {
public:
    ByteArray(size_t capacity = 0);
    virtual ~ByteArray() = default;

    ByteArray(const ByteArray& other);
    ByteArray(ByteArray&& other);
    ByteArray& operator=(ByteArray other);

    char* data();
    const char* data() const;

    size_t capacity() const;

    size_t size() const;
    void setSize(size_t size);
    bool isEmpty() const;

    void append(const void* data, size_t size);

    static ByteArray createFromRaw(const void* raw, size_t size);

private:
    void resize(size_t newCapacity);

    std::unique_ptr<char[]> data_;
    size_t capacity_;
    size_t size_;
};
}