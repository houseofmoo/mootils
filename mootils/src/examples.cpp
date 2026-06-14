// exists just to include all headers in once place to check compilation
#include "mootils/mem/byte_buffer.hpp"
#include "mootils/msg/spsc_queue.hpp"
#include "mootils/msg/spmc_queue.hpp"
#include "mootils/evt/event.hpp"
#include "mootils/evt/named_semaphore.hpp"
#include "mootils/evt/semaphore.hpp"
#include "mootils/platform/platform.hpp"
#include "mootils/print/print_safe.hpp"
#include "mootils/print/log_q.hpp"
#include "mootils/shm/shm.hpp"
#include "mootils/sock/socket_context.hpp"
#include "mootils/sock/socket_result.hpp"
#include "mootils/sock/tcp_socket.hpp"
#include "mootils/sock/udp_multicast.hpp"
#include "mootils/macros.hpp"
#include "mootils/containers/enum_array.hpp"
#include "mootils/mem/arena.hpp"
#include "mootils/seri/serialize.hpp"

void do_nothing() {
    msg::SPSCQueue<int> channel;
    auto producer_opt = channel.make_producer();
    auto consumer_opt = channel.make_consumer();

    auto producer = std::move(producer_opt.value());
    auto consumer = std::move(consumer_opt.value());

    auto push_result = producer.push(42);
    if (!push_result) {
        logr::error("CNSMR", "Failed to push item to channel");
    }

    auto pop_result = consumer.pop();
    if (pop_result) {
        logr::info("CNSMR", "Popped item: ", pop_result.value());
    } else {
        logr::error("CNSMR", "Failed to pop item from channel");
    }


    msg::SPMCQueue<int, 64, 1024> broadcast_channel;
    auto broadcast_producer_opt = broadcast_channel.make_producer();
    auto broadcast_consumer_opt = broadcast_channel.make_consumer();

    auto broadcast_producer = std::move(broadcast_producer_opt.value());
    auto broadcast_consumer = std::move(broadcast_consumer_opt.value());

    auto broadcast_push_result = broadcast_producer.push(99);
    if (!broadcast_push_result) {
        logr::error("PRDCR", "Failed to push item to broadcast channel");
    }

    auto broadcast_pop_result = broadcast_consumer.pop();
    if (broadcast_pop_result) {
        logr::info("PRDCR", "Popped item from broadcast channel: ", broadcast_pop_result.value());
    } else {
        logr::error("PRDCR", "Failed to pop item from broadcast channel");
    }

    evt::Event<int> OnMessage;
    const auto sub = OnMessage.subscribe([](int x){
        logr::info("MSG", "Received message: ", x);
    });

    OnMessage.emit(42);
}