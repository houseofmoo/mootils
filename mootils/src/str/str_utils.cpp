#include "mootils/str/str_utils.hpp"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <stdexcept>

namespace str {
    bool contains(std::string_view str, std::string_view substr) {
        return str.find(substr) != std::string_view::npos;
    }

    bool contains(std::string_view str, char c) {
        return str.find(c) != std::string_view::npos;
    }

    bool is_empty_or_whitespace(std::string_view str) {
        return std::all_of(
            str.begin(),
            str.end(),
            [](unsigned char c) {
                return std::isspace(c) != 0;
            }
        );
    }

    std::vector<std::string> split(const std::string& str, char delim, bool remove_empty) {
        std::vector<std::string> out;

        auto views = split_view(std::string_view{str}, delim, remove_empty);
        out.reserve(views.size());

        for (std::string_view token : views) {
            out.emplace_back(token);
        }

        return out;
    }

    std::vector<std::string> split(const std::string& str, std::string_view delim, bool remove_empty) {
        std::vector<std::string> out;

        auto views = split_view(std::string_view{str}, delim, remove_empty);
        out.reserve(views.size());

        for (std::string_view token : views) {
            out.emplace_back(token);
        }

        return out;
    }

    std::vector<std::string_view> split_view(std::string_view str, char delim, bool remove_empty) {
        std::vector<std::string_view> out;
        std::size_t start = 0;

        while (true) {
            const std::size_t pos = str.find(delim, start);
            const std::size_t end = pos == std::string_view::npos ? str.size() : pos;
            const std::size_t len = end - start;

            if (len > 0) {
                auto temp = str.substr(start, len);
                if (!remove_empty) {
                    // dont care about empty, insert it anyways
                    out.emplace_back(temp);
                } else {
                    // only insert if not empty
                    if (!is_empty_or_whitespace(temp)) {
                        out.emplace_back(temp);
                    }
                }
            } else {
                // found a delim with nothing, insert empty item if option selected
                if (!remove_empty) {
                    out.emplace_back("");
                }
            }

            if (pos == std::string_view::npos) {
                break;
            }

            start = pos + 1;
        }

        return out;
    }

    std::vector<std::string_view> split_view(std::string_view str, std::string_view delim, bool remove_empty) {
        std::vector<std::string_view> out;

        if (delim.empty()) {
            if (!str.empty()) {
                out.push_back(str);
            }

            return out;
        }

        std::size_t start = 0;
        while (true) {
            const std::size_t pos = str.find(delim, start);
            const std::size_t end = pos == std::string_view::npos ? str.size() : pos;
            const std::size_t len = end - start;

            if (len > 0) {
                auto temp = str.substr(start, len);
                if (!remove_empty) {
                    // dont care about empty, insert it anyways
                    out.emplace_back(temp);
                } else {
                    // only insert if not empty
                    if (!is_empty_or_whitespace(temp)) {
                        out.emplace_back(temp);
                    }
                }
            } else {
                // found a delim with nothing, insert empty item if option selected
                if (!remove_empty) {
                    out.emplace_back("");
                }
            }

            if (pos == std::string_view::npos) {
                break;
            }

            start = pos + delim.size();
        }

        return out;
    }

    std::uint32_t to_u32(std::string_view str) {
        std::uint32_t value{};

        const char* begin = str.data();
        const char* end = str.data() + str.size();

        auto result = std::from_chars(begin, end, value);

        if (result.ec != std::errc{} || result.ptr != end) {
            throw std::invalid_argument{"invalid uint32 string"};
        }

        return value;
    }

    std::int32_t to_i32(std::string_view str) {
        std::int32_t value{};

        const char* begin = str.data();
        const char* end = str.data() + str.size();

        auto result = std::from_chars(begin, end, value);

        if (result.ec != std::errc{} || result.ptr != end) {
            throw std::invalid_argument{"invalid int32 string"};
        }

        return value;
    }

    std::string_view trim_left_view(std::string_view str) {
        std::size_t start = 0;

        while (start < str.size()) {
            const auto c = static_cast<unsigned char>(str[start]);

            if (std::isspace(c) == 0) {
                break;
            }

            ++start;
        }

        return str.substr(start);
    }

    std::string_view trim_right_view(std::string_view str) {
        std::size_t end = str.size();

        while (end > 0) {
            const auto c = static_cast<unsigned char>(str[end - 1]);

            if (std::isspace(c) == 0) {
                break;
            }

            --end;
        }

        return str.substr(0, end);
    }

    std::string_view trim_view(std::string_view str) {
        return trim_right_view(trim_left_view(str));
    }

    std::string trim_left(std::string_view str) {
        return std::string{trim_left_view(str)};
    }

    std::string trim_right(std::string_view str) {
        return std::string{trim_right_view(str)};
    }

    std::string trim(std::string_view str) {
        return std::string{trim_view(str)};
    }

    bool starts_with(std::string_view str, std::string_view prefix) {
        if (prefix.size() > str.size()) {
            return false;
        }

        return str.substr(0, prefix.size()) == prefix;
    }

    bool ends_with(std::string_view str, std::string_view suffix) {
        if (suffix.size() > str.size()) {
            return false;
        }

        return str.substr(str.size() - suffix.size()) == suffix;
    }

    std::string to_lower(std::string_view str) {
        std::string out{str};

        std::transform(
            out.begin(),
            out.end(),
            out.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            }
        );

        return out;
    }

    std::string to_upper(std::string_view str) {
        std::string out{str};

        std::transform(
            out.begin(),
            out.end(),
            out.begin(),
            [](unsigned char c) {
                return static_cast<char>(std::toupper(c));
            }
        );

        return out;
    }

    std::string replace_all(std::string_view str, std::string_view from, std::string_view to) {
        if (from.empty()) {
            return std::string{str};
        }

        std::string out;
        out.reserve(str.size());
        std::size_t start = 0;

        while (true) {
            const std::size_t pos = str.find(from, start);

            if (pos == std::string_view::npos) {
                out.append(str.substr(start));
                break;
            }

            out.append(str.substr(start, pos - start));
            out.append(to);

            start = pos + from.size();
        }

        return out;
    }

    std::string join(const std::vector<std::string>& items, std::string_view delim) {
        std::string out;

        if (items.empty()) {
            return out;
        }

        std::size_t total_size = 0;

        for (const auto& item : items) {
            total_size += item.size();
        }

        total_size += delim.size() * (items.size() - 1);
        out.reserve(total_size);

        for (std::size_t i = 0; i < items.size(); ++i) {
            if (i > 0) {
                out.append(delim);
            }

            out.append(items[i]);
        }

        return out;
    }

    std::string join(const std::vector<std::string_view>& items, std::string_view delim) {
        std::string out;

        if (items.empty()) {
            return out;
        }

        std::size_t total_size = 0;

        for (std::string_view item : items) {
            total_size += item.size();
        }

        total_size += delim.size() * (items.size() - 1);
        out.reserve(total_size);

        for (std::size_t i = 0; i < items.size(); ++i) {
            if (i > 0) {
                out.append(delim);
            }

            out.append(items[i]);
        }

        return out;
    }

    bool equals_ignore_case(std::string_view a, std::string_view b) {
        if (a.size() != b.size()) {
            return false;
        }

        for (std::size_t i = 0; i < a.size(); ++i) {
            const auto ca = static_cast<unsigned char>(a[i]);
            const auto cb = static_cast<unsigned char>(b[i]);

            if (std::tolower(ca) != std::tolower(cb)) {
                return false;
            }
        }

        return true;
    }
}