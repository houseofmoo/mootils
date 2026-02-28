#if defined(MOO_WIN32)
#include "platform/platform.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include "windows_hdr.h"

namespace plat {
    int signal_sem(sem_handle sem) {
        if (sem == nullptr) {
            return static_cast<int>(ERROR_INVALID_HANDLE);
        }
        
        if (!::ReleaseSemaphore(static_cast<HANDLE>(sem), 1, nullptr)) {
            return static_cast<int>(::GetLastError());
        }

        return static_cast<int>(ERROR_SUCCESS);
    }

    int try_signal_sem(sem_handle sem) {
        // allowed to fail silently
        if (sem == nullptr) {
            return static_cast<int>(ERROR_SUCCESS);
        }
        
        if (!::ReleaseSemaphore(static_cast<HANDLE>(sem), 1, nullptr)) {
            return static_cast<int>(::GetLastError());
        }

        return static_cast<int>(ERROR_SUCCESS);
    }

    void affinitize_thread(std::thread& /*t*/, int /*cpu*/) {
        // this does not work, for some reason t.native_handle()
        // is returning a unix-like int-ish handle, not a windows
        // HANDLE

        // single cpu
        //const DWORD_PTR mask = (DWORD_PTR(1) << cpu);
        //::SetThreadAffinityMask(t.native_handle(), mask);

        // multiple cpus
        //::SetThreadAffinityMask(handle.thread, 1ull << 1 | 1ull << 2);
    }

    void affinitize_current_thread(uint32_t cpu) {
        ::SetThreadAffinityMask(::GetCurrentThread(), 1ull << cpu);
    }

    void affinitize_current_thread_to_current_cpu() {
        DWORD cpu = ::GetCurrentProcessorNumber();
        affinitize_current_thread(static_cast<uint32_t>(cpu));
    }

    std::string timestamp_str() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_s(&tm, &time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }
}
#endif