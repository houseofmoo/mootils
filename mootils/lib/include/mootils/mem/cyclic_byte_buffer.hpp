#pragma once

#include "mootils/mem/byte_buffer.hpp"
#include <cstddef>
#include <span>

namespace mem {
    // extremely simple thread UNSAFE cyclic memory block for temp storage.
    // returned spans are valid until the buffer region is reallocated from
    // a later allocate() call or free'd
    class CyclicByteBuffer {
        private:
        mem::ByteBuffer m_buf;
        std::size_t m_offset;

        public:
        explicit CyclicByteBuffer(std::size_t size) : m_buf{size}, m_offset{0} {}
        ~CyclicByteBuffer() = default;
        CyclicByteBuffer(const CyclicByteBuffer&) = delete;
        CyclicByteBuffer& operator=(const CyclicByteBuffer&) = delete;
        CyclicByteBuffer(CyclicByteBuffer&&) noexcept = default;
        CyclicByteBuffer& operator=(CyclicByteBuffer&&) noexcept = default;

        [[nodiscard]] std::byte* data() noexcept;
        [[nodiscard]] const std::byte* data() const noexcept;
        [[nodiscard]] std::size_t offset() const noexcept;
        [[nodiscard]] std::size_t remaining() const noexcept;
        [[nodiscard]] std::size_t size() const noexcept;
        [[nodiscard]] std::span<std::byte> allocate(std::size_t size) noexcept;
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] std::span<std::byte> span() noexcept;
        [[nodiscard]] std::span<const std::byte> span() const noexcept;
        void reset() noexcept;
        void clear() noexcept;

        template <typename T>
        T* as() {
            return m_buf.as<T>();
        }
    };
}
