#include "mootils/mem/cyclic_byte_buffer.hpp"

namespace mem {
    std::byte* CyclicByteBuffer::data() noexcept {
        return m_buf.data();
    }

    const std::byte* CyclicByteBuffer::data() const noexcept {
        return m_buf.data();
    }

    std::size_t CyclicByteBuffer::offset() const noexcept {
        return m_offset;
    }

    std::size_t CyclicByteBuffer::remaining() const noexcept {
        return m_buf.size() - m_offset;
    }

    std::size_t CyclicByteBuffer::size() const noexcept {
        return m_buf.size();
    }

    std::span<std::byte> CyclicByteBuffer::allocate(std::size_t size) noexcept {
        const std::size_t capacity = m_buf.size();
        if (size == 0 || size > capacity) {
            return {};
        }

        if (size > capacity - m_offset) {
            m_offset = 0;
        }

        auto out = std::span<std::byte>{m_buf.data() + m_offset, size};
        m_offset += size;
        return out;
    }

    bool CyclicByteBuffer::is_empty() const noexcept {
        return m_buf.is_empty();
    }

    std::span<std::byte> CyclicByteBuffer::as_span() noexcept {
        return m_buf.as_span();
    }

    std::span<const std::byte> CyclicByteBuffer::as_span() const noexcept {
        return m_buf.as_span();
    }

    void CyclicByteBuffer::reset() noexcept {
        m_offset = 0;
    }

    void CyclicByteBuffer::zero_out() noexcept {
        m_buf.zero_out();
    }
}