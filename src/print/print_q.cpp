#include "print/print_q.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <chrono>

namespace printq {
    struct Msg {
        LogLvl lvl;
        std::string text;
    };

    class Logger {
        private:
            std::string m_id;
            std::mutex m_mtx;
            std::chrono::steady_clock::time_point m_start_time;
            std::condition_variable m_cv;
            std::queue<Msg> m_queue;
            bool m_stopping;
            std::thread m_worker;

            void run() {
                while (true) {
                    std::queue<Msg> local; // new empty queue
                    std::string id = "NONE";
                    {
                        std::unique_lock<std::mutex> lock(m_mtx);
                        m_cv.wait(lock, [this]() { 
                            return m_stopping || !m_queue.empty(); 
                        });

                        if (m_stopping && m_queue.empty()) { break; }
                        local.swap(m_queue); // swap contents so we can print without blocking producers
                        id = m_id; // capture the id at the time of printing
                    }

                    while (!local.empty()) {
                        auto ms = static_cast<std::uint64_t>(
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - m_start_time
                            ).count()
                        );

                        auto& out = local.front().lvl == LogLvl::Error ? std::cerr : std::cout;
                        out << "[ " << id << " | " << ms << "ms ] " << local.front().text << "\n";
                        local.pop();
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
                m_id{"NONE"}, 
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

            void set_id(std::string id) noexcept {
                std::lock_guard<std::mutex> lock(m_mtx);
                m_id = id;
            }

            void enqueue(const LogLvl lvl, std::string msg) {
                {
                    std::lock_guard<std::mutex> lock(m_mtx);
                    if (m_stopping) { return; }
                    m_queue.push(Msg{ lvl, std::move(msg) });
                }
                m_cv.notify_one();
            }
    };
    static Logger logger{};

    void set_id(std::string id) {
        logger.set_id(std::move(id));
    }

    namespace detail {
        void enqueue(const LogLvl lvl, std::string msg) { 
            logger.enqueue(lvl, std::move(msg));
        }
    }
}