#if defined(MOO_LINUX)
#include "platform/platform.h"
#include <semaphore.h>
#include <pthread.h>
#include <sched.h>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace plat {
    int signal_sem(sem_handle sem) {
        if (sem == nullptr) { 
            return EINVAL;
        }

        if (::sem_post(static_cast<sem_t*>(sem)) == -1) {
            return errno;
        }

        return 0;
    }

    int try_signal_sem(sem_handle sem) {
        // allowed to fail silently
        if (sem == nullptr) { 
            return 0;
        }

        if (::sem_post(static_cast<sem_t*>(sem)) == -1) {
            return errno;
        }

        return 0;
    }

    void affinitize_thread(std::thread& t, uint32_t cpu) {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(cpu, &set);
        ::pthread_setaffinity_np(t.native_handle(), sizeof(set), &set);
    }

    void affinitize_current_thread(uint32_t cpu) {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(cpu, &set);
        ::pthread_setaffinity_np(pthread_self(), sizeof(set), &set);
    }

    void affinitize_current_thread_to_current_cpu() {
        int cpu = sched_getcpu();
        if (cpu >= 0) {
            plat::affinitize_current_thread(static_cast<uint32_t>(cpu));
        }
    }

    std::string timestamp_str() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm tm{};
        localtime_r(&time, &tm);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }
}
#endif