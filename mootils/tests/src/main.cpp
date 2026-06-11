#include <thread>
#include <array>
#include <iostream>
#include "mootils/print/print_safe.hpp"
#include "mootils/evt/semaphore.hpp"

#include "print/print_tests.hpp"
#include "evt/event_tests.hpp"

struct TestWorker {
    std::thread thread;
    evt::Semaphore test_complete;
};

int main() {
    std::array<TestWorker, 2> workers;
    workers[0].thread = std::thread([&]() {
        logger_q_test();
        auto result = workers[0].test_complete.post();
        if (!result.ok()) {
            ERR("print test: failed to post test completion: ", result.code_to_string(), " during ", result.op_to_string());
        }
    });
    auto wresult = workers[0].test_complete.wait();
    if (!wresult.ok()) {
        ERR("print test: failed to wait for test completion: ", wresult.code_to_string(), " during ", wresult.op_to_string());
        return 1;
    }
    workers[0].thread.join();

    workers[1].thread = std::thread([&]() {
        test_events();
        auto result = workers[1].test_complete.post();
        if (!result.ok()) {
            ERR("event test: failed to post test completion: ", result.code_to_string(), " during ", result.op_to_string());
        }
    });
    wresult = workers[1].test_complete.wait();
    if (!wresult.ok()) {
        ERR("event test: failed to wait for test completion: ", wresult.code_to_string(), " during ", wresult.op_to_string());
    }
    workers[1].thread.join();

    // test_named_semaphore();
    // test_semaphore();

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "All tests complete" << std::endl;
    return 0;
}

