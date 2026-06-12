#pragma once

#include <memory>
#include <cstddef>
#include <span>

namespace mem {
    // extremely simple thread UNSAFE memory block for temp storage
    class ByteBuffer {
        private:
        std::unique_ptr<std::byte[]> m_buf;
        std::size_t m_size;

        public:
        explicit ByteBuffer(std::size_t size);
        ~ByteBuffer() = default;
        ByteBuffer(const ByteBuffer&) = delete;
        ByteBuffer& operator=(const ByteBuffer&) = delete;
        ByteBuffer(ByteBuffer&&) noexcept = default;
        ByteBuffer& operator=(ByteBuffer&&) noexcept = default;

        [[nodiscard]] std::byte* data() noexcept;
        [[nodiscard]] const std::byte* data() const noexcept;
        [[nodiscard]] std::size_t size() const noexcept;
        [[nodiscard]] bool is_empty() const noexcept;
        [[nodiscard]] std::span<std::byte> as_span() noexcept;
        [[nodiscard]] std::span<const std::byte> as_span() const noexcept;
        void zero_out() noexcept;

        template <typename T>
        T* as() {
            return reinterpret_cast<T*>(m_buf.get());
        }
    };
}
