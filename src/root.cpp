// exists just to include all headers in once place to check compilation
#include <iostream>
#include "msg/spsc_queue.h"
#include "msg/spmc_queue.h"
#include "evt/event.h"
#include "evt/named_semaphore.h"
#include "evt/semaphore.h"
#include "platform/platform.h"
#include "print/print.h"
#include "shm/shm.h"
#include "sock/socket_context.h"
#include "sock/socket_result.h"
#include "sock/tcp_socket.h"
#include "sock/udp_multicast.h"
#include "macros.h"

inline void do_nothing() {
    msg::SPSCQueue<int> channel;
    auto producer_opt = channel.make_producer();
    auto consumer_opt = channel.make_consumer();

    auto producer = std::move(producer_opt.value());
    auto consumer = std::move(consumer_opt.value());

    auto push_result = producer.push(42);
    if (!push_result) {
        std::cerr << "Failed to push item to channel\n";
    }

    auto pop_result = consumer.pop();
    if (pop_result) {
        std::cout << "Popped item: " << pop_result.value() << "\n";
    } else {
        std::cerr << "Failed to pop item from channel\n";
    }


    msg::SPMCQueue<int, 64, 1024> broadcast_channel;
    auto broadcast_producer_opt = broadcast_channel.make_producer();
    auto broadcast_consumer_opt = broadcast_channel.make_consumer();

    auto broadcast_producer = std::move(broadcast_producer_opt.value());
    auto broadcast_consumer = std::move(broadcast_consumer_opt.value());

    auto broadcast_push_result = broadcast_producer.push(99);
    if (!broadcast_push_result) {
        std::cerr << "Failed to push item to broadcast channel\n";
    }

    auto broadcast_pop_result = broadcast_consumer.pop();
    if (broadcast_pop_result) {
        std::cout << "Popped item from broadcast channel: " << broadcast_pop_result.value() << "\n";
    } else {
        std::cerr << "Failed to pop item from broadcast channel\n";
    }

    evt::Event<int> OnMessage;
    const auto sub = OnMessage.subscribe([](int x){
        std::cout << x << std::endl;
    });

    OnMessage.emit(42);

}