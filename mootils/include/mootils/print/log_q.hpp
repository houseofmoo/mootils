#pragma once

#include <string>
#include <cstdint>
#include <sstream>
#include <utility>
#include <string_view>
#include "mootils/api.hpp"

namespace logr {
    namespace detail {
        enum class LogLvl : std::uint8_t {
            Debug = 0,
            Info = 1,
            Warning = 2,
            Error = 3,
            None = 99, // disable all logging
        };

        #if !defined(MOO_LOGQ_LOG_LEVEL)
            #pragma message("DEFAULT LOG LEVEL: DEBUG")
            inline constexpr LogLvl compile_time_min_level = LogLvl::Debug;
        #else
            static_assert(
                MOO_LOGQ_LOG_LEVEL == 0 ||
                MOO_LOGQ_LOG_LEVEL == 1 ||
                MOO_LOGQ_LOG_LEVEL == 2 ||
                MOO_LOGQ_LOG_LEVEL == 3 ||
                MOO_LOGQ_LOG_LEVEL == 99,
                "Invalid MOO_LOGQ_LOG_LEVEL. Use 0 (Debug), 1 (Info), 2 (Warning), 3 (Error), or 99 (None)"
            );
            inline constexpr LogLvl compile_time_min_level = static_cast<LogLvl>(MOO_LOGQ_LOG_LEVEL);
        #endif

        MOOTILS_API void enqueue(std::string src, LogLvl lvl, std::string msg);

        template <LogLvl Lvl>
        inline constexpr bool enabled() {
            return static_cast<std::uint8_t>(Lvl) >=
                   static_cast<std::uint8_t>(compile_time_min_level);
        }

        template <LogLvl Lvl, typename... Args>
        inline void print_impl(std::string_view src, Args&&... args) {
            if constexpr (enabled<Lvl>()) {
                std::ostringstream oss;
                (oss << ... << std::forward<Args>(args));
                detail::enqueue(std::string(src), Lvl, oss.str());
            } else {
                (void)src;
                ((void)args, ...);
            }
        }
    }

    template <typename... Args>
    inline void debug(std::string_view src, Args&&... args) {
        detail::print_impl<detail::LogLvl::Debug>(src, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void info(std::string_view src, Args&&... args) {
        detail::print_impl<detail::LogLvl::Info>(src, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void warn(std::string_view src, Args&&... args) {
        detail::print_impl<detail::LogLvl::Warning>(src, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline void error(std::string_view src, Args&&... args) {
        detail::print_impl<detail::LogLvl::Error>(src, std::forward<Args>(args)...);
    }
}