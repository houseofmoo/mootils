#pragma once
#include <bit>
#include <cstdint>
#include <type_traits>

namespace mem {
    namespace {
        template <typename T>
        inline constexpr bool is_byte_swappable_integral_v =
            std::is_integral_v<T> &&
            !std::is_same_v<std::remove_cv_t<T>, bool> &&
            !std::is_same_v<std::remove_cv_t<T>, char> &&
            !std::is_same_v<std::remove_cv_t<T>, signed char> &&
            !std::is_same_v<std::remove_cv_t<T>, unsigned char> &&
            !std::is_same_v<std::remove_cv_t<T>, wchar_t> &&
            !std::is_same_v<std::remove_cv_t<T>, char8_t> &&
            !std::is_same_v<std::remove_cv_t<T>, char16_t> &&
            !std::is_same_v<std::remove_cv_t<T>, char32_t>;
    }

    template <typename T>
    T byte_swap16(T val) {
        static_assert(sizeof(T) == 2, "T must be 16 bits");
        static_assert(std::is_integral_v<T>, "T must be integral type");
        //static_assert(std::is_byte_swappable_integral_v<T>, "T must be integral type");

        auto bits = std::bit_cast<std::uint16_t>(val);
        bits = static_cast<std::uint16_t>(
            ((bits & 0x00ffu) << 8) |
            ((bits & 0xff00u) >> 8)
        );
        return std::bit_cast<T>(bits);
    }

    template <typename T>
    T byte_swap32(T val) {
        static_assert(sizeof(T) == 4, "T must be 32 bits");
        static_assert(std::is_integral_v<T>, "T must be integral type");
        //static_assert(std::is_byte_swappable_integral_v<T>, "T must be integral type");

        auto bits = std::bit_cast<std::uint32_t>(val);
        bits = static_cast<std::uint32_t>(
            ((bits & 0x000000ffu) << 24) |
            ((bits & 0x0000ff00u) << 8)  |
            ((bits & 0x00ff0000u) >> 8)  |
            ((bits & 0xff000000u) >> 24)
        );
        return std::bit_cast<T>(bits);
    }

    template <typename T>
    T byte_swap64(T val) {
        static_assert(sizeof(T) == 8, "T must be 64 bits");
        static_assert(std::is_integral_v<T>, "T must be integral type");
        //static_assert(std::is_byte_swappable_integral_v<T>, "T must be integral type");

        auto bits = std::bit_cast<std::uint64_t>(val);
        bits = static_cast<std::uint64_t>(
            ((bits & 0x00000000000000ffull) << 56) |
            ((bits & 0x000000000000ff00ull) << 40) |
            ((bits & 0x0000000000ff0000ull) << 24) |
            ((bits & 0x00000000ff000000ull) << 8)  |
            ((bits & 0x000000ff00000000ull) >> 8)  |
            ((bits & 0x0000ff0000000000ull) >> 24) |
            ((bits & 0x00ff000000000000ull) >> 40) |
            ((bits & 0xff00000000000000ull) >> 56)
        );
        return std::bit_cast<T>(bits);
    }

    float byte_swapf(float val) {
        static_assert(sizeof(float) == 4, "floats must be 32 bits");

        auto bits = std::bit_cast<std::uint32_t>(val);
        bits = byte_swap32(bits);
        return std::bit_cast<float>(bits);
    }

    double byte_swapd(double val) {
        static_assert(sizeof(double) == 8, "double must be 64 bits");

        auto bits = std::bit_cast<std::uint64_t>(val);
        bits = byte_swap64(bits);
        return std::bit_cast<double>(bits);
    }
}