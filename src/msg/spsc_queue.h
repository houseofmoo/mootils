#pragma once
#include <array>
#include <cstdint>
#include <cassert>
#include <atomic>
#include <optional>
#include <type_traits>
#include <utility>

namespace msg {
    // Lock free single producer single consumer queue
    // NOTE: Producer and Consumer must not outlive SPSCQueue instance
    template <typename T, std::size_t NumSlots = 1024>
    class SPSCQueue {
        static_assert(NumSlots > 0, "SPSCQueue size must be > 0");
        static_assert((NumSlots & (NumSlots-1)) == 0, "SPSCQueue size must be a power of 2 for efficient modulo operation");
        static_assert(std::is_trivially_copyable_v<T>, "SPSCQueue only supports trivially copyable types");

        static constexpr std::size_t mask = NumSlots - 1;
        static constexpr std::size_t to_index(std::uint64_t i) noexcept { return static_cast<std::size_t>(i) & mask; }

        private:
            std::array<T, NumSlots> m_buffer{};
            alignas(64) std::atomic<std::uint64_t> m_head{0};
            alignas(64) std::atomic<std::uint64_t> m_tail{0};

            std::atomic<bool> m_producer_claimed{false};
            std::atomic<bool> m_consumer_claimed{false};

            [[nodiscard]] bool push_impl(const T& item) noexcept {
                // get the current head and tail values
                const auto head = m_head.load(std::memory_order_relaxed);
                const auto tail = m_tail.load(std::memory_order_acquire);

                if (head - tail >= NumSlots) {
                    return false; // channel is full
                }

                m_buffer[to_index(head)] = item;
                m_head.store(head + 1, std::memory_order_release);
                return true;
            }

            [[nodiscard]] bool push_impl(T&& item) noexcept {
                // get the current head and tail values
                const auto head = m_head.load(std::memory_order_relaxed);
                const auto tail = m_tail.load(std::memory_order_acquire);

                if (head - tail >= NumSlots) {
                    return false; // channel is full
                }

                m_buffer[to_index(head)] = std::move(item);
                m_head.store(head + 1, std::memory_order_release);
                return true;
            }

            [[nodiscard]] std::optional<T> pop_impl() noexcept {
                // get the current head and tail values
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_tail.load(std::memory_order_relaxed);

                if (tail >= head) {
                    return std::nullopt; // channel is empty
                }
                
                T out = m_buffer[to_index(tail)];
                m_tail.store(tail + 1, std::memory_order_release);
                return out;
            }

            [[nodiscard]] bool try_pop_impl(T& out) noexcept {
                // get the current head and tail values
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_tail.load(std::memory_order_relaxed);

                if (tail >= head) {
                    return false; // channel is empty
                }
                
                out = m_buffer[to_index(tail)];
                m_tail.store(tail + 1, std::memory_order_release);
                return true;
            }

            [[nodiscard]] std::optional<T> peek() const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_tail.load(std::memory_order_acquire);

                if (tail >= head) {
                    return std::nullopt;
                }
                
                return m_buffer[to_index(tail)];
            }

            [[nodiscard]] bool peek(T& out) const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_tail.load(std::memory_order_acquire);

                if (tail >= head) {
                    return false;
                }
                
                out = m_buffer[to_index(tail)];
                return true;
            }

            [[nodiscard]] std::size_t count_snapshot_impl() const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_tail.load(std::memory_order_acquire);
                const auto diff = head - tail;
                if (diff > NumSlots) {
                    return NumSlots; // overflow
                }
                return static_cast<std::size_t>(head - tail);
            }

        public:
            SPSCQueue() = default;
            ~SPSCQueue() = default;

            SPSCQueue(const SPSCQueue&) = delete;
            SPSCQueue& operator=(const SPSCQueue&) = delete;
            SPSCQueue(SPSCQueue&&) = delete;
            SPSCQueue& operator=(SPSCQueue&&) = delete;

            class Producer {
                private:
                    SPSCQueue* m_queue = nullptr;

                public:
                    explicit Producer(SPSCQueue& q) noexcept : m_queue(&q) {}
                    
                    ~Producer() {
                        if (m_queue != nullptr) {
                            m_queue->m_producer_claimed.store(false, std::memory_order_release);
                        }
                    };

                    Producer(const Producer&) = delete;
                    Producer& operator=(const Producer&) = delete;
                    
                    Producer(Producer&& other) noexcept : m_queue(other.m_queue) {
                        other.m_queue = nullptr;
                    }

                    Producer& operator=(Producer&& other) noexcept {
                        if (this != &other) {
                            if (m_queue) {
                                m_queue->m_producer_claimed.store(false, std::memory_order_release);
                            }
                            m_queue = other.m_queue;
                            other.m_queue = nullptr;
                        }
                        return *this;
                    }

                    [[nodiscard]] bool push(const T& item) noexcept {
                        assert(m_queue != nullptr && "Invalid producer: queue is null");
                        return m_queue->push_impl(item);
                    }

                    [[nodiscard]] bool push(T&& item) noexcept {
                        assert(m_queue != nullptr && "Invalid producer: queue is null");
                        return m_queue->push_impl(std::move(item));
                    }

                    std::size_t count_snapshot() const noexcept {
                        assert(m_queue != nullptr && "Invalid producer: queue is null");
                        return m_queue->count_snapshot_impl();
                    }
            };

            class Consumer {
                private:
                    SPSCQueue* m_queue = nullptr;

                public:
                    explicit Consumer(SPSCQueue& q) noexcept : m_queue(&q) {}
                    
                    ~Consumer() {
                        if (m_queue != nullptr) {
                            m_queue->m_consumer_claimed.store(false, std::memory_order_release);
                        }
                    }

                    Consumer(const Consumer&) = delete;
                    Consumer& operator=(const Consumer&) = delete;
                    
                    Consumer(Consumer&& other) noexcept : m_queue(other.m_queue) {
                        other.m_queue = nullptr;
                    }

                    Consumer& operator=(Consumer&& other) noexcept {
                        if (this != &other) {
                            if (m_queue) {
                                m_queue->m_consumer_claimed.store(false, std::memory_order_release);
                            }
                            m_queue = other.m_queue;
                            other.m_queue = nullptr;
                        }
                        return *this;
                    }

                    std::optional<T> pop() noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->pop_impl();
                    }

                    [[nodiscard]] bool try_pop(T& out) noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->try_pop_impl(out);
                    }

                    std::optional<T> peek() const noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->peek();
                    }

                    [[nodiscard]] bool peek(T& out) const noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->peek(out);
                    }

                    std::size_t count_snapshot() const noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->count_snapshot_impl();
                    }
            };

            [[nodiscard]] std::optional<Producer> make_producer() noexcept {
                bool expected = false;
                if (!m_producer_claimed.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                    return std::nullopt;
                }
                return Producer(*this);
            }

            [[nodiscard]] std::optional<Consumer> make_consumer() noexcept {
                bool expected = false;
                if (!m_consumer_claimed.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                    return std::nullopt;
                }
                return Consumer(*this);
            }
    };
}