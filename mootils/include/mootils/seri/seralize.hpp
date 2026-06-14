#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <string>
#include <type_traits>
#include <limits>
#include <cassert>
#include "mootils/api.hpp"

namespace seri {
    // inherit to create your own serializer for any data type that needs to be sent over the wire
    template <typename T>
    class Serializer {
        public:
        virtual ~Serializer() = default;

        // implement a serialization strategy to convert from obj T into std::bytes.
        // ByteWriter can be used as a helper for this implementation
        virtual void serialize(T obj, std::span<std::byte> to_buf) = 0;

        // implement a deserialization strategy to convert from std::bytes into obj T.
        // ByteReader can be used as a helper for this implementation
        virtual void deserialize(T obj, std::span<std::byte> from_buf) = 0;

        // return the expected size of obj T when converted to std::bytes
        virtual std::size_t predict_size(T obj) = 0;
    };

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

    // returns the expected size in bytes of a serialized value
    class SizeCounter {
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
