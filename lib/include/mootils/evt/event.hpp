#pragma once
#include <functional>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <mutex>
#include <atomic>

namespace evt {
    // Threadsafe C# style event subscription system
    // NOTE: Subscription should not outlive Event
    template <typename... Args>
    class Event {
        private:
            struct Handle {
                std::uint64_t id;
                std::function<void(Args...)> callback;
            };

            mutable std::mutex m_mutex;
            std::vector<Handle> m_handles;
            std::atomic<std::uint64_t> m_next_id{0};

            void unsubscribe(std::uint64_t id) noexcept {
                std::lock_guard lock(m_mutex);
                auto it = std::remove_if(m_handles.begin(), m_handles.end(),
                                        [&](const Handle& h) { return h.id == id; });
                m_handles.erase(it, m_handles.end());
            }

        public:
            class Subscription {
                public:
                    Subscription() = default;
                    ~Subscription() { unsubscribe(); }

                    Subscription(const Subscription&) = delete;
                    Subscription& operator=(const Subscription&) = delete;

                    Subscription(Subscription&& other) noexcept
                        : m_event(other.m_event), m_id(other.m_id) {
                        other.m_event = nullptr;
                        other.m_id = 0;
                    }

                    Subscription& operator=(Subscription&& other) noexcept {
                        if (this != &other) {
                            unsubscribe();
                            m_event = other.m_event;
                            m_id = other.m_id;
                            other.m_event = nullptr;
                            other.m_id = 0;
                        }
                        return *this;
                    }

                    void unsubscribe() noexcept {
                        if (m_event != nullptr) {
                            m_event->unsubscribe(m_id);
                            m_event = nullptr;
                            m_id = 0;
                        }
                    }

                    explicit operator bool() const noexcept { return m_event != nullptr; }

                private:
                    friend class Event;
                    Subscription(Event* e, std::uint64_t id) : m_event(e), m_id(id) {}

                    Event* m_event = nullptr;
                    std::uint64_t m_id = 0;
            };

            Event() = default;
            ~Event() = default;

            Event(const Event&) = delete;
            Event& operator=(const Event&) = delete;
            Event(Event&&) = delete;
            Event& operator=(Event&&) = delete;

            template <typename F>
            [[nodiscard]] Subscription subscribe(F&& f) {
                std::lock_guard lock(m_mutex);
                const std::uint64_t id = m_next_id.fetch_add(1, std::memory_order_relaxed);
                m_handles.push_back(Handle{id, std::function<void(Args...)>(std::forward<F>(f))});
                return Subscription(this, id);
            }

            void emit(Args... args) {
                std::vector<Handle> snapshot;
                {
                    std::lock_guard lock(m_mutex);
                    snapshot = m_handles;
                }
                
                for (auto& handle : snapshot) {
                    if (handle.callback) handle.callback(args...);
                }
            }

            std::size_t subscriber_count() const noexcept {
                std::lock_guard lock(m_mutex);
                return m_handles.size(); 
            }
    };
}