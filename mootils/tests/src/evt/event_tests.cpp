#include "evt/event_tests.hpp"
#include <thread>
#include <chrono>
#include "mootils/print/print_safe.hpp"
#include "mootils/evt/event.hpp"
#include "mootils/evt/named_semaphore.hpp"
#include "mootils/evt/semaphore.hpp"

bool test_events() {
    INFO("Event test starts");
    evt::Event<int> event;

    int call_count = 0;
    auto sub1 = event.subscribe([&](int x) {
        call_count++;
        INFO("Sub1 called with ", x);
    });

    auto sub2 = event.subscribe([&](int x) {
        call_count++;
        INFO("Sub2 called with ", x);
    });

    event.emit(42);
    if (call_count != 2) {
        ERR("Expected call count 2, got ", call_count);
        return false;
    }

    // remove 1 subscriber and emit again
    sub1.unsubscribe();
    event.emit(84);
    if (call_count != 3) {
        ERR("Expected call count 3, got ", call_count);
        return false;
    }

    // remove last subscriber and emit again
    sub2.unsubscribe();
    event.emit(168);
    if (call_count != 3) {
        ERR("Expected call count 3, got ", call_count);
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    INFO("Event test completes");
    return true;
}

bool test_named_semaphore() {
    return true;
}

bool test_semaphore() {
    return true;
}