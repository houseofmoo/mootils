#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <string>
#include <type_traits>
#include "mootils/api.hpp"

namespace seri::byte {
    // reads little-endian byte representation from provided buffer
    class ByteReader {
    private:
        std::span<const std::byte> m_buf;
        std::size_t m_offset{};

        bool read_bytes(void* dst, std::size_t size);

    public:
        MOOTILS_API explicit ByteReader(std::span<const std::byte> buf);

        MOOTILS_API [[nodiscard]] std::size_t size() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t used() const noexcept;
        MOOTILS_API [[nodiscard]] std::size_t remaining() const noexcept;
        MOOTILS_API [[nodiscard]] bool empty() const noexcept;
        MOOTILS_API void reset() noexcept;

        MOOTILS_API bool read_u8(std::uint8_t& out);
        MOOTILS_API bool read_i8(std::int8_t& out);
        MOOTILS_API bool read_byte(std::byte& out);
        MOOTILS_API bool read_bool(bool& out);

        MOOTILS_API bool read_u16(std::uint16_t& out);
        MOOTILS_API bool read_i16(std::int16_t& out);

        MOOTILS_API bool read_u32(std::uint32_t& out);
        MOOTILS_API bool read_i32(std::int32_t& out);

        MOOTILS_API bool read_u64(std::uint64_t& out);
        MOOTILS_API bool read_i64(std::int64_t& out);

        MOOTILS_API bool read_float(float& out);
        MOOTILS_API bool read_double(double& out);

        MOOTILS_API bool read_char(char& out);
        MOOTILS_API bool read_string_view(std::string_view& out);
        MOOTILS_API bool read_string(std::string& out);

        template <typename Enum>
        bool read_enum32(Enum& out) {
            static_assert(std::is_enum_v<Enum>, "read_enum32 is for enums only");
            static_assert(sizeof(std::underlying_type_t<Enum>) == 4,
                          "read_enum32 is for 32-bit enums only");

            std::uint32_t raw{};
            if (!read_u32(raw)) {
                return false;
            }

            using U = std::underlying_type_t<Enum>;
            out = static_cast<Enum>(static_cast<U>(raw));
            return true;
        }

        template <typename Enum>
        bool read_enum16(Enum& out) {
            static_assert(std::is_enum_v<Enum>, "read_enum16 is for enums only");
            static_assert(sizeof(std::underlying_type_t<Enum>) == 2,
                          "read_enum16 is for 16-bit enums only");

            std::uint16_t raw{};
            if (!read_u16(raw)) {
                return false;
            }

            using U = std::underlying_type_t<Enum>;
            out = static_cast<Enum>(static_cast<U>(raw));
            return true;
        }

        template <typename Enum>
        bool read_enum8(Enum& out) {
            static_assert(std::is_enum_v<Enum>, "read_enum8 is for enums only");
            static_assert(sizeof(std::underlying_type_t<Enum>) == 1,
                          "read_enum8 is for 8-bit enums only");

            std::uint8_t raw{};
            if (!read_u8(raw)) {
                return false;
            }

            using U = std::underlying_type_t<Enum>;
            out = static_cast<Enum>(static_cast<U>(raw));
            return true;
        }
    };
}