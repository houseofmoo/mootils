#include "print/log_q.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <chrono>

namespace logr {
    struct Msg {
        std::string_view src;
        LogLvl lvl;
        std::string text;
    };

    class Logger {
        private:
            std::mutex m_mtx;
            std::chrono::steady_clock::time_point m_start_time;
            std::condition_variable m_cv;
            std::queue<Msg> m_queue;
            bool m_stopping;
            std::thread m_worker;

            void run() {
                std::queue<Msg> local; // local queue to swap between producer and consumer to minimize lock time

                while (true) {
                    {
                        std::unique_lock<std::mutex> lock(m_mtx);
                        m_cv.wait(lock, [this]() { 
                            return m_stopping || !m_queue.empty(); 
                        });

                        if (m_stopping && m_queue.empty()) { break; }
                        local.swap(m_queue); // swap contents so we can print without blocking producers
                    }

                    while (!local.empty()) {
                        Msg msg = std::move(local.front());
                        local.pop();
                        
                        auto ms = static_cast<std::uint64_t>(
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - m_start_time
                            ).count()
                        );

                        auto& out = msg.lvl == LogLvl::Error ? std::cerr : std::cout;
                        out << "[ " << ms << " | " << msg.src << "] " << msg.text << "\n";
                    }

                    std::cout.flush();
                    std::cerr.flush();
                }
            }

            void shutdown() {
                {
                    std::lock_guard<std::mutex> lock(m_mtx);
                    m_stopping = true;
                }
                m_cv.notify_all();

                if (m_worker.joinable()) {
                    m_worker.join();
                }
            }
        
        public:
            Logger() : 
                m_mtx{}, 
                m_start_time(std::chrono::steady_clock::now()),
                m_cv{}, 
                m_queue{},
                m_stopping{false},
                m_worker{[this]() { run(); }} {}

            ~Logger() { shutdown(); }

            Logger(const Logger&) = delete;
            Logger& operator=(const Logger&) = delete;
            Logger(Logger&&) = delete;
            Logger& operator=(Logger&&) = delete;

            void enqueue(const std::string_view src, const LogLvl lvl, std::string msg) {
                {
                    std::lock_guard<std::mutex> lock(m_mtx);
                    if (m_stopping) { return; }
                    m_queue.push(Msg{ src, lvl, std::move(msg) });
                }
                m_cv.notify_one();
            }
    };
    static Logger logger{};

    namespace detail {
        void enqueue(const std::string_view src, const LogLvl lvl, std::string msg) { 
            logger.enqueue(src,lvl, std::move(msg));
        }
    }
}