#pragma once

#include "mootils/mem/byte_buffer.hpp"
#include <cstddef>
#include <span>
#include "mootils/api.hpp"

namespace mem {
    // extremely simple thread UNSAFE cyclic memory block for temp storage.
    // returned spans are valid until the buffer region is reallocated from
    // a later allocate() call or free'd
    class CyclicByteBuffer {
        private:
        mem::ByteBuffer m_buf;
        std::size_t m_offset;

        public:
        MOOTILS_API explicit CyclicByteBuffer(std::size_t size);
        MOOTILS_API ~CyclicByteBuffer();
        CyclicByteBuffer(const CyclicByteBuffer&) = delete;
        CyclicByteBuffer& operator=(const CyclicByteBuffer&) = delete;
        MOOTILS_API CyclicByteBuffer(CyclicByteBuffer&&) noexcept;
        MOOTILS_API CyclicByteBuffer& operator=(CyclicByteBuffer&&) noexcept;

        MOOTILS_API [[nodiscard]] std::byte* data() noexcept;
        MOOTILS_API [[nodiscard]] const std::byte* data() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t offset() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t remaining() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t size() const noexcept;
        MOOTILS_API [[nodiscard]] std::span<std::byte> allocate(std::size_t size) noexcept;
        MOOTILS_API [[nodiscard]] bool is_empty() const noexcept;
        MOOTILS_API [[nodiscard]] std::span<std::byte> as_span() noexcept;
        MOOTILS_API [[nodiscard]] std::span<const std::byte> as_span() const noexcept;
        MOOTILS_API void reset() noexcept;
        MOOTILS_API void zero_out() noexcept;

        template <typename T>
        T* as() {
            return m_buf.as<T>();
        }
    };
}
