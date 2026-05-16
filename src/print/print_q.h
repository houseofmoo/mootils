#pragma once

#include <string>
#include <cstdint>
#include <sstream>
#include <string_view>
#include <utility>

namespace printq {
    enum class LogLvl : std::uint8_t {
        Debug,   // only print in debug builds
        Info,    // print in both debug and release builds
        Warning, // print in both debug and release builds, but to stderr
        Error,   // print in both debug and release builds, but to stderr
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

        void enqueue(const LogLvl lvl, std::string msg);

        template <LogLvl Lvl>
        inline constexpr bool enabled() {
            return static_cast<std::uint8_t>(Lvl) >=
                   static_cast<std::uint8_t>(compile_time_min_level);
        }

        template <LogLvl Lvl, typename... Args>
        inline void print_impl(Args&&... args) {
            if constexpr (enabled<Lvl>()) {
                std::ostringstream oss;
                oss << loglvl_to_string(Lvl) << " ";
                (oss << ... << std::forward<Args>(args));
                detail::enqueue(Lvl, oss.str());
            }
        }
    }

    void set_id(std::string id);

    template <typename... Args>
    inline void debug(Args&&... args) {
        detail::print_impl<LogLvl::Debug>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(Args&&... args) {
        detail::print_impl<LogLvl::Info>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warning(Args&&... args) {
        detail::print_impl<LogLvl::Warning>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(Args&&... args) {
        detail::print_impl<LogLvl::Error>(std::forward<Args>(args)...);
    }
}