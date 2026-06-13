#pragma once

#include <memory>
#include <cstddef>
#include <span>
#include <optional>
#include "mootils/api.hpp"

namespace mem {
    // a simple thread UNSAFE arena allocator. returns spans
    // to memory within arena, valid until the arena is reset
    class Arena {
        private:
        std::unique_ptr<std::byte[]> m_buf;
        std::size_t m_size{0};
        std::size_t m_offset{0};

        public:
        MOOTILS_API explicit Arena(std::size_t size);
        MOOTILS_API ~Arena();
        Arena(const Arena&) = delete;
        Arena& operator=(const Arena&) = delete;
        Arena(Arena&&) noexcept = delete;
        Arena& operator=(Arena&&) noexcept = delete;

        MOOTILS_API [[nodiscard]] std::optional<std::span<std::byte>> allocate(std::size_t size) noexcept;
        MOOTILS_API [[nodiscard]] std::size_t size() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t remaining() const noexcept;
        void MOOTILS_API reset() noexcept;
        void MOOTILS_API zero_out() noexcept;
    };
}