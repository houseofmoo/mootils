#pragma once

#include <string>
#include <cstdint>

namespace evt {
    enum class NamedSemErr {
        None = 0,           // Success
        DoubleOpen,         // Called open() twice on this named event
        NotInitialized,     // Event not created / already closed
        InvalidName,        // Name could not be constructed or rejected
        OpenFailed,         // OS failed to create/open named event
        SignalFailed,       // post()/SetEvent failed
        Timeout,            // Timed wait expired
        WouldBlock,
        MaxCount,
        SysError,           // wait() failed (WAIT_FAILED, etc.)
    };

    enum class NamedSemOp {
        Open,
        Post,
        TryWait,
        Wait
    };
    
    struct NamedSemResult {
        NamedSemErr code;
        NamedSemOp op;

        bool ok() const noexcept {
            return code == NamedSemErr::None;
        }

        std::string_view code_to_string() const noexcept {
            switch (code) {
                case NamedSemErr::None: return "None";
                case NamedSemErr::DoubleOpen: return "DoubleOpen";
                case NamedSemErr::NotInitialized: return "NotInitialized";
                case NamedSemErr::InvalidName: return "InvalidName";
                case NamedSemErr::OpenFailed: return "OpenFailed";
                case NamedSemErr::SignalFailed: return "SignalFailed";
                case NamedSemErr::Timeout: return "Timeout";
                case NamedSemErr::WouldBlock: return "WouldBlock";
                case NamedSemErr::MaxCount: return "MaxCount";
                case NamedSemErr::SysError: return "SysError";
                default: return "Unknown - error is undefined";
            }
        }

        std::string_view op_to_string() const noexcept {
            switch (op) {
                case NamedSemOp::Open: return "Open";
                case NamedSemOp::Post: return "Post";
                case NamedSemOp::TryWait: return "TryWait";
                case NamedSemOp::Wait: return "Wait";
                default: return "Unknown - op is undefined";
            }
        }
    };

    constexpr std::int64_t INVALID_ID = -1;
    #if defined(MOO_WIN32)
        using sem_handle = void*;
    #elif defined(MOO_LINUX)
        using sem_handle = void*;
    #endif

    // Counting named semaphore
    class NamedSemaphore {
        private:
            std::int64_t m_dst_id; 
            sem_handle m_sem;

        public:
            NamedSemaphore(std::int64_t id);
            ~NamedSemaphore();

            NamedSemaphore(const NamedSemaphore&) = delete;
            NamedSemaphore& operator=(const NamedSemaphore&) = delete;
            NamedSemaphore(NamedSemaphore&& other) noexcept;
            NamedSemaphore& operator=(NamedSemaphore&& other) noexcept;
            
            std::string name() const;
            std::int64_t get_dst_id() const { return m_dst_id; }
            [[nodiscard]] NamedSemResult open();
            [[nodiscard]] NamedSemResult post() const;
            [[nodiscard]] NamedSemResult try_wait() const;
            [[nodiscard]] NamedSemResult wait(std::uint32_t milliseconds = 0) const;
            void close();

        private: 
    };
}