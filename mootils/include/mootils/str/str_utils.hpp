#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace str {
    bool contains(std::string_view str, std::string_view substr);
    bool contains(std::string_view str, char c);

    bool is_empty_or_whitespace(std::string_view str);

    std::vector<std::string> split(const std::string& str, char delim, bool remove_empty = true);
    std::vector<std::string> split(const std::string& str, std::string_view delim, bool remove_empty = true);

    std::vector<std::string_view> split_view(std::string_view str, char delim, bool remove_empty = true);
    std::vector<std::string_view> split_view(std::string_view str, std::string_view delim, bool remove_empty = true);

    std::uint32_t to_u32(std::string_view str);
    std::int32_t to_i32(std::string_view str);

    std::string_view trim_left_view(std::string_view str);
    std::string_view trim_right_view(std::string_view str);
    std::string_view trim_view(std::string_view str);

    std::string trim_left(std::string_view str);
    std::string trim_right(std::string_view str);
    std::string trim(std::string_view str);

    bool starts_with(std::string_view str, std::string_view prefix);
    bool ends_with(std::string_view str, std::string_view suffix);

    std::string to_lower(std::string_view str);
    std::string to_upper(std::string_view str);

    std::string replace_all(std::string_view str, std::string_view from, std::string_view to);

    std::string join(const std::vector<std::string>& items, std::string_view delim);
    std::string join(const std::vector<std::string_view>& items, std::string_view delim);

    bool equals_ignore_case(std::string_view a, std::string_view b);
}