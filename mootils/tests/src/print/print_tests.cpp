#include "print/print_tests.hpp"

#include "mootils/print/print_safe.hpp"
#include "mootils/print/log_q.hpp"
#include <thread>
#include <chrono>
#include <cstdint>

bool logger_q_test() {
    INFO("Logger test starts");
    std::uint32_t counter = 0;
    while (counter < 30) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        switch (counter % 4) {
            case 0:    
                logr::debug("TEST", "This is a debug message: ", counter);
                break;
            case 1:
                logr::info("TEST", "This is an info message: ", counter);
                break;
            case 2:
                logr::warn("TEST", "This is a warning message: ", counter);
                break;
            case 3:
                logr::error("TEST", "This is an error message: ", counter);
                break;
        }
        counter++;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    INFO("Logger test complete");
    return true;
}