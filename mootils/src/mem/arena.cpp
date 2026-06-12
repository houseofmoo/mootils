#include "mootils/mem/arena.hpp"

namespace mem {
    Arena::Arena(std::size_t size) :
        m_buf{std::make_unique<std::byte[]>(size)},
        m_size{size},
        m_offset{0} { }

    std::optional<std::span<std::byte>> Arena::allocate(std::size_t size) noexcept {
        if (size == 0 || size > remaining()) {
            return std::nullopt;
        }

        std::size_t current_offset = m_offset;
        m_offset += size;
        return std::span<std::byte>{m_buf.get() + current_offset, size};
    }

    void Arena::reset() noexcept {
        m_offset = 0;
    }

    std::size_t Arena::size() const noexcept {
        return m_size;
    }

    std::size_t Arena::remaining() const noexcept {
        return m_size - m_offset;
    }
}