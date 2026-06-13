#pragma once
#include <string_view>
#include <cstdint>
#include "mootils/api.hpp"

namespace evt {
    enum class SemErr {
        None = 0,
        NotInitialized,
        Timeout,
        WouldBlock,
        MaxCountReached,
        SysError,
    };

    enum class SemOp {
        Post,
        TryWait,
        Wait
    };

    struct SemResult {
        SemErr code;
        SemOp op;

        bool ok() const noexcept {
            return code == SemErr::None;
        }

        std::string_view code_to_string() const noexcept {
            switch (code) {
                case SemErr::None: return "None";
                case SemErr::NotInitialized: return "NotInitialized";
                case SemErr::Timeout: return "Timeout";
                case SemErr::WouldBlock: return "WouldBlock";
                case SemErr::MaxCountReached: return "MaxCountReached";
                case SemErr::SysError: return "SysError";
                default: return "Unknown - error is undefined";
            }
        }

        std::string_view op_to_string() const noexcept {
            switch (op) {
                case SemOp::Post: return "Post";
                case SemOp::TryWait: return "TryWait";
                case SemOp::Wait: return "Wait";
                default: return "Unknown - op is undefined";
            }
        }
    };

    #if defined(MOO_WIN32)
        using sem_handle = void*;
    #elif defined(MOO_LINUX)
        using sem_handle = void*;
    #endif

    // Counting semaphore
    class Semaphore {
        private:
            sem_handle m_sem;
            std::uint32_t m_max_count = 0;

        public:
            MOOTILS_API Semaphore();
            MOOTILS_API explicit Semaphore(std::uint32_t max_count);
            MOOTILS_API ~Semaphore();

            Semaphore(const Semaphore&) = delete;
            Semaphore& operator=(const Semaphore&) = delete;
            MOOTILS_API Semaphore(Semaphore&& other) noexcept;
            MOOTILS_API Semaphore& operator=(Semaphore&& other) noexcept;

            MOOTILS_API [[nodiscard]] SemResult post();
            MOOTILS_API [[nodiscard]] SemResult try_wait();
            MOOTILS_API [[nodiscard]] SemResult wait(std::uint32_t milliseconds = 0);
            MOOTILS_API void close();
    };
}