#include "mootils/shm/shm.hpp"
#include <thread>
#include <chrono>
#include <cstring>
#include <cstddef>
#include <span>

namespace shm {
    void Shm::memset(size_t offset, int32_t val,std::size_t bytes) {
        if (offset+ bytes > m_total_size) return;
        std::memset(
            static_cast<std::byte*>(m_view) + offset,
            static_cast<int>(val),
            bytes
        );
    }

   std::size_t Shm::total_size() const noexcept {
        return m_total_size;
    }

    ShmResult Shm::read(void* to_buf, const std::size_t read_size, const std::size_t offset_into) const noexcept {
        if (!is_valid()) return { ShmErr::NotOpen, ShmOp::Read };
        if (read_size > m_total_size) return { ShmErr::TooLarge, ShmOp::Read };
        if (offset_into + read_size > m_total_size) return { ShmErr::InvalidOffset, ShmOp::Read };

        std::memcpy(to_buf, static_cast<std::byte*>(m_view) + offset_into, read_size);
        return { ShmErr::None, ShmOp::Read };
    }

    ShmResult Shm::read(const std::span<std::byte> to_blob, const std::size_t offset_into) const noexcept {
        return read(to_blob.data(), to_blob.size_bytes(), offset_into);
    }

    ShmResult Shm::write(const void* from_buf, const std::size_t write_size, const std::size_t offset_into) noexcept {
        if (!is_valid()) return { ShmErr::NotOpen, ShmOp::Write };
        if (write_size > m_total_size) return{ ShmErr::TooLarge, ShmOp::Write };
        if (offset_into + write_size > m_total_size) return { ShmErr::InvalidOffset, ShmOp::Write };

        std::memcpy(static_cast<std::byte*>(m_view) + offset_into, from_buf, write_size);
        return { ShmErr::None, ShmOp::Write };
    }

    ShmResult Shm::write(const std::span<std::byte> from_blob, const std::size_t offset_into) noexcept {
        return write(from_blob.data(), from_blob.size_bytes(), offset_into);
    }
}