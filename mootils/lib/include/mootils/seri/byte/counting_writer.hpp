#pragma once
#include <cstdint>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <limits>
#include <cassert>

namespace seri::byte {
    // returns the expected size of a serialized value
    class CountingWriter {
        private:
        std::size_t m_used = 0;

        template <typename T>
        std::size_t add_size() noexcept {
            constexpr std::size_t size = sizeof(T);
            m_used += size;
            return size;
        }

        public:
        void reset() noexcept { m_used = 0; }
        std::size_t used() const noexcept { return m_used; }
        std::size_t write_u8(std::uint8_t) noexcept { return add_size<std::uint8_t>(); }
        std::size_t write_i8(std::int8_t) noexcept { return add_size<std::int8_t>(); }
        std::size_t write_byte(std::byte) noexcept { return add_size<std::byte>(); }
        std::size_t write_bool(bool) noexcept { return add_size<std::uint8_t>(); }
        std::size_t write_char(char) noexcept { return add_size<char>(); }

        std::size_t write_u16(std::uint16_t) noexcept { return add_size<std::uint16_t>(); }
        std::size_t write_i16(std::int16_t) noexcept { return add_size<std::int16_t>(); }

        std::size_t write_u32(std::uint32_t) noexcept { return add_size<std::uint32_t>(); }
        std::size_t write_i32(std::int32_t) noexcept { return add_size<std::int32_t>(); }

        std::size_t write_u64(std::uint64_t) noexcept { return add_size<std::uint64_t>(); }
        std::size_t write_i64(std::int64_t) noexcept { return add_size<std::int64_t>(); }

        std::size_t write_float(float) noexcept {
            static_assert(sizeof(float) == 4);
            return add_size<std::uint32_t>();
        }

        std::size_t write_double(double) noexcept {
            static_assert(sizeof(double) == 8);
            return add_size<std::uint64_t>();
        }

        std::size_t write_string(const std::string_view str) noexcept {
            // when we're going to write a string we alway write the size of the string before
            // the string itself
            assert(str.size() <= std::numeric_limits<std::uint32_t>::max());
            constexpr std::size_t str_size_len = sizeof(std::uint32_t);
            std::size_t str_len = str.size();
            m_used += (str_size_len + str_len);
            return str_size_len + str_len;
        }

        template <typename Enum>
        std::size_t write_enum32(Enum) noexcept {
            static_assert(std::is_enum_v<Enum>, "write_enum32 is for enums only");
            static_assert(sizeof(Enum) == 4, "write_enum32 is for 32 bit enums only");
            return add_size<std::uint32_t>();
        }

        template <typename Enum>
        std::size_t write_enum16(Enum) noexcept {
            static_assert(std::is_enum_v<Enum>, "write_enum16 is for enums only");
            static_assert(sizeof(Enum) == 2, "write_enum16 is for 16 bit enums only");
            return add_size<std::uint16_t>();
        }

        template <typename Enum>
        std::size_t write_enum8(Enum) noexcept {
            static_assert(std::is_enum_v<Enum>, "write_enum8 is for enums only");
            static_assert(sizeof(Enum) == 1, "write_enum8 is for 8 bit enums only");
            return add_size<std::uint8_t>();
        }
    };
}