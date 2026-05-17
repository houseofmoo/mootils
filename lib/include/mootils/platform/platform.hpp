#pragma once
#include <thread>
#include <string>

namespace plat {
    #if defined(MOO_WIN32)
        using sem_handle = void*;
    #elif defined(MOO_LINUX)
        using sem_handle = void*;
    #endif

    [[nodiscard]] int signal_sem(sem_handle sem);
    [[nodiscard]] int try_signal_sem(sem_handle sem);

    void affinitize_thread(std::thread& t, uint32_t cpu);
    void affinitize_current_thread(uint32_t cpu);
    void affinitize_current_thread_to_current_cpu();
    std::string timestamp_str();
}