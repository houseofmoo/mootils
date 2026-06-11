#pragma once
#include <array>
#include <cstdint>
#include <atomic>
#include <optional>
#include <type_traits>
#include <utility>
#include <cassert>

namespace msg {
    // Lock free single producer multiple consumer broadcast queue
    // NOTE: Producer and Consumer must not outlive SPMCQueue instance
    template <typename T, std::size_t NumSlots = 1024, std::size_t MaxConsumers = 16>
    class SPMCQueue {
        static_assert(NumSlots > 0, "SPMCQueue size must be > 0");
        static_assert((NumSlots & (NumSlots-1)) == 0, "SPMCQueue size must be a power of 2 for efficient modulo operation");
        static_assert(std::is_trivially_copyable_v<T>, "SPMCQueue only supports trivially copyable types");
        static_assert(MaxConsumers > 0, "SPMCQueue MaxConsumers must be > 0");
        
        static constexpr std::size_t mask = NumSlots - 1;
        static constexpr std::size_t to_index(std::uint64_t i) noexcept { return static_cast<std::size_t>(i) & mask; }

        private:
            enum class SlotState : uint8_t {
                Free = 0,
                Initializing = 1,
                Active = 2
            };

            struct alignas(64) ConsumerSlot  {
                std::atomic<SlotState> state{SlotState::Free};
                std::atomic<std::uint64_t> tail{0};
            };

            std::array<T, NumSlots> m_buffer{};
            alignas(64) std::atomic<std::uint64_t> m_head{0};
            std::array<ConsumerSlot , MaxConsumers> m_slots{};
            std::atomic<bool> m_producer_claimed{false};

            std::uint64_t min_tail_snapshot(std::uint64_t head) const noexcept {
                std::uint64_t min_tail = head;
                bool any = false;

                for (const auto& slot : m_slots) {
                    if (slot.state.load(std::memory_order_acquire) == SlotState::Active) {
                        any = true;
                        const auto t = slot.tail.load(std::memory_order_relaxed);
                        if (t < min_tail) min_tail = t;
                    }
                }
                return any ? min_tail : head;
            }

            [[nodiscard]] bool push_impl(const T& item) noexcept {
                const auto head = m_head.load(std::memory_order_relaxed);
                const auto min_tail = min_tail_snapshot(head);

                if (head - min_tail >= NumSlots) {
                    return false; // queue is full
                }

                m_buffer[to_index(head)] = item;
                m_head.store(head + 1, std::memory_order_release);
                return true;
            }

            [[nodiscard]] bool push_impl(T&& item) noexcept {
                const auto head = m_head.load(std::memory_order_relaxed);
                const auto min_tail = min_tail_snapshot(head);

                if (head - min_tail >= NumSlots) {
                    return false; // queue is full
                }

                m_buffer[to_index(head)] = std::move(item);
                m_head.store(head + 1, std::memory_order_release);
                return true;
            }

            [[nodiscard]] std::optional<T> pop_impl(std::size_t idx) noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_slots[idx].tail.load(std::memory_order_relaxed);

                if (tail >= head) {
                    return std::nullopt; // queue is empty
                }
                
                T out = m_buffer[to_index(tail)];
                m_slots[idx].tail.store(tail + 1, std::memory_order_release);
                return out;
            }

            [[nodiscard]] bool try_pop_impl(T& out, std::size_t idx) noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_slots[idx].tail.load(std::memory_order_relaxed);

                if (tail >= head) {
                    return false; // queue is empty
                }
                
                out = m_buffer[to_index(tail)];
                m_slots[idx].tail.store(tail + 1, std::memory_order_release);
                return true;
            }

            [[nodiscard]] std::optional<T> peek(std::size_t idx) const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_slots[idx].tail.load(std::memory_order_relaxed);

                if (tail >= head) {
                    return std::nullopt; // queue is empty
                }
                
                return m_buffer[to_index(tail)];
            }

            [[nodiscard]] bool peek(T& out, std::size_t idx) const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_slots[idx].tail.load(std::memory_order_relaxed);

                if (tail >= head) {
                    return false; // queue is empty
                }
                
                out = m_buffer[to_index(tail)];
                return true;
            }

            [[nodiscard]] std::size_t count_snapshot_impl(std::size_t idx) const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto tail = m_slots[idx].tail.load(std::memory_order_relaxed);

                const auto diff = head - tail;
                if (diff > NumSlots) { 
                    return NumSlots; // overflow
                }
                return static_cast<std::size_t>(diff);
            }

            [[nodiscard]] std::size_t count_snapshot_impl() const noexcept {
                const auto head = m_head.load(std::memory_order_acquire);
                const auto min_tail = min_tail_snapshot(head);

                const auto diff = head - min_tail;
                if (diff > NumSlots) { 
                    return NumSlots; // overflow
                }
                return static_cast<std::size_t>(diff);
            }
        
        public:
            SPMCQueue() = default;
            ~SPMCQueue() = default;

            SPMCQueue(const SPMCQueue&) = delete;
            SPMCQueue& operator=(const SPMCQueue&) = delete;
            SPMCQueue(SPMCQueue&&) = delete;
            SPMCQueue& operator=(SPMCQueue&&) = delete;

            class Producer {
                private:
                    SPMCQueue* m_queue = nullptr;

                public:
                    explicit Producer(SPMCQueue& q) noexcept : m_queue(&q) {}
                    
                    ~Producer() {
                        if (m_queue != nullptr) {
                            m_queue->m_producer_claimed.store(false, std::memory_order_release);
                        }
                    }

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
                        // returns the number of items in the queue based
                        // on the slowest consumer
                        assert(m_queue != nullptr && "Invalid producer: queue is null");
                        return m_queue->count_snapshot_impl();
                    }
            };

            class Consumer {
                private:
                    SPMCQueue* m_queue = nullptr;
                    std::size_t m_slot_idx = 0;

                public:
                    Consumer() = default;
                    Consumer(SPMCQueue& q, std::size_t slot_idx) noexcept : 
                        m_queue(&q), m_slot_idx(slot_idx) {}

                    ~Consumer() {
                        if (m_queue != nullptr) {
                            m_queue->m_slots[m_slot_idx].tail.store(0, std::memory_order_relaxed);
                            m_queue->m_slots[m_slot_idx].state.store(SlotState::Free, std::memory_order_release);
                        }
                    }

                    Consumer(const Consumer&) = delete;
                    Consumer& operator=(const Consumer&) = delete;
                    
                    Consumer(Consumer&& other) noexcept : 
                        m_queue(other.m_queue), m_slot_idx(other.m_slot_idx) {
                        other.m_queue = nullptr;
                    }
                    
                    Consumer& operator=(Consumer&& other) noexcept {
                        if (this != &other) {
                            if (m_queue != nullptr) {
                                m_queue->m_slots[m_slot_idx].state.store(SlotState::Free, std::memory_order_release);
                            }

                            m_queue = other.m_queue;
                            m_slot_idx = other.m_slot_idx;
                            other.m_queue = nullptr;
                        }
                        return *this;
                    }

                    std::optional<T> pop() noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->pop_impl(m_slot_idx);
                    }

                    [[nodiscard]] bool try_pop(T& out) noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->try_pop_impl(out, m_slot_idx);
                    }

                    std::optional<T> peek() const noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->peek(m_slot_idx);
                    }

                    [[nodiscard]] bool peek(T& out) const noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->peek(out, m_slot_idx);
                    }

                    std::size_t count_snapshot() const noexcept {
                        assert(m_queue != nullptr && "Invalid consumer: queue is null");
                        return m_queue->count_snapshot_impl(m_slot_idx);
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
                const auto head = m_head.load(std::memory_order_acquire);

                for (std::size_t i = 0; i < MaxConsumers; ++i) {
                    SlotState expected = SlotState::Free;
                    if (m_slots[i].state.compare_exchange_strong(expected, SlotState::Initializing, std::memory_order_acq_rel)) {
                        m_slots[i].tail.store(head, std::memory_order_relaxed);
                        m_slots[i].state.store(SlotState::Active, std::memory_order_release);
                        return Consumer(*this, i);
                    }
                }

                return std::nullopt; // no free consumer slots
            }
    };
}