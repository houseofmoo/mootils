#pragma once
#include <thread>
#include <string>
#include "mootils/api.hpp"

namespace plat {
    #if defined(MOO_WIN32)
        using sem_handle = void*;
    #elif defined(MOO_LINUX)
        using sem_handle = void*;
    #endif

    MOOTILS_API [[nodiscard]] int signal_sem(sem_handle sem);
    MOOTILS_API [[nodiscard]] int try_signal_sem(sem_handle sem);

    MOOTILS_API void affinitize_thread(std::thread& t, uint32_t cpu);
    MOOTILS_API void affinitize_current_thread(uint32_t cpu);
    MOOTILS_API void affinitize_current_thread_to_current_cpu();
    MOOTILS_API std::string timestamp_str();
}