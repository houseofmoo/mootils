#pragma once

#include <cstdint>
#include <cstddef>
#include <string_view>
#include <span>
#include <type_traits>
#include "mootils/api.hpp"

namespace seri::byte {
    // writes byte representation as little endian to provided buffer
    class ByteWriter {
        private:
        std::span<std::byte> m_buf;
        std::size_t m_offset;

        std::size_t write_bytes(const void* src, std::size_t size);

        public:
        MOOTILS_API explicit ByteWriter(std::span<std::byte> buf);

        MOOTILS_API [[nodiscard]] std::size_t size() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t used() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t space_available() const noexcept;
        MOOTILS_API [[nodiscard]] std::span<std::byte> written() const noexcept;
        MOOTILS_API void reset() noexcept;

        MOOTILS_API std::size_t write_u8(const std::uint8_t val);
        MOOTILS_API std::size_t write_i8(const std::int8_t val);
        MOOTILS_API std::size_t write_byte(const std::byte val);
        MOOTILS_API std::size_t write_bool(const bool val);

        MOOTILS_API std::size_t write_u16(const std::uint16_t val);
        MOOTILS_API std::size_t write_i16(const std::int16_t val);

        MOOTILS_API std::size_t write_u32(const std::uint32_t val);
        MOOTILS_API std::size_t write_i32(const std::int32_t val);

        MOOTILS_API std::size_t write_u64(const std::uint64_t val);
        MOOTILS_API std::size_t write_i64(const std::int64_t val);

        MOOTILS_API std::size_t write_float(const float val);
        MOOTILS_API std::size_t write_double(const double val);

        MOOTILS_API std::size_t write_char(const char val);
        MOOTILS_API std::size_t write_string(const std::string_view str);

        template <typename Enum>
        std::size_t write_enum32(const Enum val) noexcept {
            static_assert(std::is_enum_v<Enum>, "write_enum32 is for enums only");
            static_assert(sizeof(Enum) == 4, "write_enum32 is for 32 bit enums only");
            return write_u32(static_cast<std::uint32_t>(val));
        }

        template <typename Enum>
        std::size_t write_enum16(const Enum val) noexcept {
            static_assert(std::is_enum_v<Enum>, "write_enum16 is for enums only");
            static_assert(sizeof(Enum) == 2, "write_enum16 is for 16 bit enums only");
            return write_u16(static_cast<std::uint16_t>(val));
        }

        template <typename Enum>
        std::size_t write_enum8(const Enum val) noexcept {
            static_assert(std::is_enum_v<Enum>, "write_enum8 is for enums only");
            static_assert(sizeof(Enum) == 1, "write_enum8 is for 8 bit enums only");
            return write_u8(static_cast<std::uint8_t>(val));
        }
    };
}
