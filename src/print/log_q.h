#pragma once

#include <string>
#include <cstdint>
#include <sstream>
#include <string_view>
#include <utility>

namespace logr {
    enum class LogLvl : std::uint8_t {
        Debug = 0, // only print in debug builds to stdout
        Info,      // print in both debug and release builds to stdout
        Warning,   // print in both debug and release builds but to stderr
        Error,     // print in both debug and release builds but to stderr
    };

#ifdef NDEBUG // release build
    inline constexpr LogLvl compile_time_min_level = LogLvl::Info;
#else
    inline constexpr LogLvl compile_time_min_level = LogLvl::Debug;
#endif

    namespace detail {
        inline std::string_view loglvl_to_string(LogLvl lvl) {
            switch (lvl) {
                case LogLvl::Debug:   return "DEBUG";
                case LogLvl::Info:    return "INFO ";
                case LogLvl::Warning: return "WARN ";
                case LogLvl::Error:   return "ERROR";
                default:              return "UNKNOWN";
            }
        }

        void enqueue(const std::string_view src, const LogLvl lvl, std::string msg);

        template <LogLvl Lvl>
        inline constexpr bool enabled() {
            return static_cast<std::uint8_t>(Lvl) >=
                   static_cast<std::uint8_t>(compile_time_min_level);
        }

        template <LogLvl Lvl, typename... Args>
        inline void print_impl(const std::string_view src, Args&&... args) {
            if constexpr (enabled<Lvl>()) {
                std::ostringstream oss;
                oss << loglvl_to_string(Lvl) << " ";
                (oss << ... << std::forward<Args>(args));
                detail::enqueue(src, Lvl, oss.str());
            }
        }
    }

    template <typename... Args>
    inline void debug(Args&&... args) {
        detail::print_impl<LogLvl::Debug>("UNKN", std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(Args&&... args) {
        detail::print_impl<LogLvl::Info>("UNKN", std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warning(Args&&... args) {
        detail::print_impl<LogLvl::Warning>("UNKN", std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(Args&&... args) {
        detail::print_impl<LogLvl::Error>("UNKN", std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void debug(const std::string_view src, Args&&... args) {
        detail::print_impl<LogLvl::Debug>(src, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(const std::string_view src, Args&&... args) {
        detail::print_impl<LogLvl::Info>(src, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warning(const std::string_view src, Args&&... args) {
        detail::print_impl<LogLvl::Warning>(src, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(const std::string_view src, Args&&... args) {
        detail::print_impl<LogLvl::Error>(src, std::forward<Args>(args)...);
    }
}