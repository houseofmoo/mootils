#include "mootils/mem/byte_buffer.hpp"
#include <algorithm>

namespace mem {
    ByteBuffer::ByteBuffer(std::size_t size)
        : m_buf{std::make_unique<std::byte[]>(size)}, m_size{size} {}

    std::byte* ByteBuffer::data() noexcept {
        return m_buf.get();
    }

    const std::byte* ByteBuffer::data() const noexcept {
        return m_buf.get();
    }

    std::size_t ByteBuffer::size() const noexcept {
        return m_size;
    }

    bool ByteBuffer::empty() const noexcept {
        return m_size == 0;
    }

    std::span<std::byte> ByteBuffer::span() noexcept {
        return {m_buf.get(), m_size};
    }

    std::span<const std::byte> ByteBuffer::span() const noexcept {
        return {m_buf.get(), m_size};
    }

    void ByteBuffer::clear() noexcept {
        std::fill_n(m_buf.get(), m_size, std::byte{0});
    }
}