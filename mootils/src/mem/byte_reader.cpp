#include "mootils/mem/serializer.hpp"

#include <bit>
#include <cstring>

namespace mem {
    ByteReader::ByteReader(std::span<const std::byte> buf)
        : m_buf{buf}, m_offset{0} {}

    std::size_t ByteReader::size() const noexcept {
        return m_buf.size();
    }

    std::size_t ByteReader::used() const noexcept {
        return m_offset;
    }

    std::size_t ByteReader::remaining() const noexcept {
        return m_buf.size() - m_offset;
    }

    bool ByteReader::empty() const noexcept {
        return remaining() == 0;
    }

    void ByteReader::reset() noexcept {
        m_offset = 0;
    }

    bool ByteReader::read_bytes(void* dst, std::size_t size) {
        if (remaining() < size) {
            return false;
        }

        std::memcpy(dst, m_buf.data() + m_offset, size);
        m_offset += size;
        return true;
    }

    bool ByteReader::read_u8(std::uint8_t& out) {
        std::byte b{};

        if (!read_bytes(&b, sizeof(b))) {
            return false;
        }

        out = std::to_integer<std::uint8_t>(b);
        return true;
    }

    bool ByteReader::read_i8(std::int8_t& out) {
        std::uint8_t raw{};

        if (!read_u8(raw)) {
            return false;
        }

        out = static_cast<std::int8_t>(raw);
        return true;
    }

    bool ByteReader::read_byte(std::byte& out) {
        return read_bytes(&out, sizeof(out));
    }

    bool ByteReader::read_bool(bool& out) {
        if (remaining() < 1) {
            return false;
        }

        const auto raw = std::to_integer<std::uint8_t>(m_buf[m_offset]);

        if (raw > 1) {
            return false;
        }

        ++m_offset;
        out = raw != 0;
        return true;
    }

    bool ByteReader::read_u16(std::uint16_t& out) {
        std::byte bytes[2]{};

        if (!read_bytes(bytes, sizeof(bytes))) {
            return false;
        }

        out =
            (std::uint16_t{std::to_integer<std::uint8_t>(bytes[0])} << 0) |
            (std::uint16_t{std::to_integer<std::uint8_t>(bytes[1])} << 8);

        return true;
    }

    bool ByteReader::read_i16(std::int16_t& out) {
        std::uint16_t raw{};

        if (!read_u16(raw)) {
            return false;
        }

        out = static_cast<std::int16_t>(raw);
        return true;
    }

    bool ByteReader::read_u32(std::uint32_t& out) {
        std::byte bytes[4]{};

        if (!read_bytes(bytes, sizeof(bytes))) {
            return false;
        }

        out =
            (std::uint32_t{std::to_integer<std::uint8_t>(bytes[0])} << 0)  |
            (std::uint32_t{std::to_integer<std::uint8_t>(bytes[1])} << 8)  |
            (std::uint32_t{std::to_integer<std::uint8_t>(bytes[2])} << 16) |
            (std::uint32_t{std::to_integer<std::uint8_t>(bytes[3])} << 24);

        return true;
    }

    bool ByteReader::read_i32(std::int32_t& out) {
        std::uint32_t raw{};

        if (!read_u32(raw)) {
            return false;
        }

        out = static_cast<std::int32_t>(raw);
        return true;
    }

    bool ByteReader::read_u64(std::uint64_t& out) {
        std::byte bytes[8]{};

        if (!read_bytes(bytes, sizeof(bytes))) {
            return false;
        }

        out =
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[0])} << 0)  |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[1])} << 8)  |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[2])} << 16) |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[3])} << 24) |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[4])} << 32) |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[5])} << 40) |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[6])} << 48) |
            (std::uint64_t{std::to_integer<std::uint8_t>(bytes[7])} << 56);

        return true;
    }

    bool ByteReader::read_i64(std::int64_t& out) {
        std::uint64_t raw{};

        if (!read_u64(raw)) {
            return false;
        }

        out = static_cast<std::int64_t>(raw);
        return true;
    }

    bool ByteReader::read_float(float& out) {
        static_assert(sizeof(float) == 4);

        std::uint32_t raw{};

        if (!read_u32(raw)) {
            return false;
        }

        out = std::bit_cast<float>(raw);
        return true;
    }

    bool ByteReader::read_double(double& out) {
        static_assert(sizeof(double) == 8);

        std::uint64_t raw{};

        if (!read_u64(raw)) {
            return false;
        }

        out = std::bit_cast<double>(raw);
        return true;
    }

    bool ByteReader::read_char(char& out) {
        std::uint8_t raw{};

        if (!read_u8(raw)) {
            return false;
        }

        out = static_cast<char>(raw);
        return true;
    }

    bool ByteReader::read_string_view(std::string_view& out) {
        const std::size_t start_offset = m_offset;
        std::uint32_t len{};

        if (!read_u32(len)) {
            return false;
        }

        if (remaining() < len) {
            m_offset = start_offset;
            return false;
        }

        const auto* data = reinterpret_cast<const char*>(m_buf.data() + m_offset);
        out = std::string_view{data, len};
        m_offset += len;
        return true;
    }

    bool ByteReader::read_string(std::string& out) {
        std::string_view view{};
        if (!read_string_view(view)) {
            return false;
        }
        out.assign(view.data(), view.size());
        return true;
    }
}