#pragma once

#include <memory>
#include <cstddef>
#include <span>
#include "mootils/api.hpp"

namespace mem {
    // extremely simple thread UNSAFE memory block for temp storage
    class ByteBuffer {
        private:
        std::unique_ptr<std::byte[]> m_buf;
        std::size_t m_size;

        public:
        MOOTILS_API explicit ByteBuffer(std::size_t size);
        MOOTILS_API  ~ByteBuffer();
        ByteBuffer(const ByteBuffer&) = delete;
        ByteBuffer& operator=(const ByteBuffer&) = delete;
        MOOTILS_API ByteBuffer(ByteBuffer&&) noexcept;
        MOOTILS_API ByteBuffer& operator=(ByteBuffer&&) noexcept;

        MOOTILS_API [[nodiscard]] std::byte* data() noexcept;
        MOOTILS_API [[nodiscard]] const std::byte* data() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t size() const noexcept;
        MOOTILS_API [[nodiscard]] bool is_empty() const noexcept;
        MOOTILS_API [[nodiscard]] std::span<std::byte> as_span() noexcept;
        MOOTILS_API [[nodiscard]] std::span<const std::byte> as_span() const noexcept;
        MOOTILS_API void zero_out() noexcept;

        template <typename T>
        T* as() {
            return reinterpret_cast<T*>(m_buf.get());
        }
    };
}
