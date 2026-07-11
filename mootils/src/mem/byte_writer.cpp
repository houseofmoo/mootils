#include "mootils/mem/serializer.hpp"
#include <cstring>
#include <limits>
#include <bit>

namespace mem {
    ByteWriter::ByteWriter(std::span<std::byte> buf) : m_buf{buf}, m_offset{0} {}

    std::size_t ByteWriter::size() const noexcept {
        return m_buf.size();
    }

    std::size_t ByteWriter::used() const noexcept {
        return m_offset;
    }

    std::size_t ByteWriter::space_available() const noexcept {
        return m_buf.size() - m_offset;
    }

    std::span<std::byte> ByteWriter::written() const noexcept {
        return m_buf.subspan(0, m_offset);
    }

    void ByteWriter::reset() noexcept {
        m_offset = 0;
    }

    std::size_t ByteWriter::write_bytes(const void* src, std::size_t size) {
        if (space_available() < size) {
            return 0;
        }

        std::memcpy(m_buf.data() + m_offset, src, size);
        m_offset += size;
        return size;
    }

    std::size_t ByteWriter::write_u8(const std::uint8_t val) {
        std::byte b = static_cast<std::byte>(val);
        return write_bytes(&b, sizeof(std::byte));
    }

    std::size_t ByteWriter::write_i8(const std::int8_t val) {
        return write_u8(static_cast<std::uint8_t>(val));
    }

    std::size_t ByteWriter::write_byte(const std::byte val) {
        return write_bytes(&val, sizeof(std::byte));
    }

    std::size_t ByteWriter::write_bool(const bool val) {
        return write_u8(val ? 1 : 0);
    }

    std::size_t ByteWriter::write_u16(const std::uint16_t val) {
        std::byte bytes[2]{
            static_cast<std::byte>((val >> 0) & 0xFF),
            static_cast<std::byte>((val >> 8) & 0xFF),
        };
        return write_bytes(bytes, sizeof(bytes));
    }

    std::size_t ByteWriter::write_i16(const std::int16_t val) {
        return write_u16(static_cast<std::uint16_t>(val));
    }

    std::size_t ByteWriter::write_u32(const std::uint32_t val) {
        std::byte bytes[4]{
            static_cast<std::byte>((val >> 0)  & 0xFF),
            static_cast<std::byte>((val >> 8)  & 0xFF),
            static_cast<std::byte>((val >> 16) & 0xFF),
            static_cast<std::byte>((val >> 24) & 0xFF),
        };
        return write_bytes(bytes, sizeof(bytes));
    }

    std::size_t ByteWriter::write_i32(const std::int32_t val) {
        return write_u32(static_cast<std::uint32_t>(val));
    }

    std::size_t ByteWriter::write_u64(const std::uint64_t val) {
    std::byte bytes[8]{
            static_cast<std::byte>((val >> 0)  & 0xFF),
            static_cast<std::byte>((val >> 8)  & 0xFF),
            static_cast<std::byte>((val >> 16) & 0xFF),
            static_cast<std::byte>((val >> 24) & 0xFF),
            static_cast<std::byte>((val >> 32) & 0xFF),
            static_cast<std::byte>((val >> 40) & 0xFF),
            static_cast<std::byte>((val >> 48) & 0xFF),
            static_cast<std::byte>((val >> 56) & 0xFF),
        };
        return write_bytes(bytes, sizeof(bytes));
    }

    std::size_t ByteWriter::write_i64(const std::int64_t val) {
        return write_u64(static_cast<std::uint64_t>(val));
    }

    std::size_t ByteWriter::write_float(const float val) {
        static_assert(sizeof(float) == 4);
        return write_u32(std::bit_cast<std::uint32_t>(val));
    }

    std::size_t ByteWriter::write_double(const double val) {
        static_assert(sizeof(double) == 8);
        return write_u64(std::bit_cast<std::uint64_t>(val));
    }

    std::size_t ByteWriter::write_char(char val) {
        return write_u8(static_cast<std::uint8_t>(val));
    }

    std::size_t ByteWriter::write_string(const std::string_view str) {
        if (str.size() > std::numeric_limits<std::uint32_t>::max()) {
            return 0;
        }

        // confirm enough room for both size and string chars
        if (space_available() < sizeof(std::uint32_t) + str.size()) {
            return 0;
        }

        std::size_t total_size = write_u32(static_cast<std::uint32_t>(str.size()));
        total_size += write_bytes(str.data(), str.size());
        return total_size;
    }
}