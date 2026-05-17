#include "mootils/print/log_q.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <chrono>

namespace logr {
    struct Msg {
        std::string src;
        detail::LogLvl lvl;
        std::string text;
    };

    class Logger {
        public:
            std::mutex m_mtx;
            std::chrono::steady_clock::time_point m_start_time;
            std::condition_variable m_cv;
            std::queue<Msg> m_queue;
            bool m_stopping;
            std::thread m_worker;
        
            Logger() : 
                m_mtx{}, 
                m_start_time(std::chrono::steady_clock::now()),
                m_cv{}, 
                m_queue{},
                m_stopping{false},
                m_worker{[this]() { run(); }} {
                    std::cout << "[ LOGGER ] started: " << std::endl;
                    m_worker.detach();
                }

            ~Logger() { shutdown(); }

            Logger(const Logger&) = delete;
            Logger& operator=(const Logger&) = delete;
            Logger(Logger&&) = delete;
            Logger& operator=(Logger&&) = delete;

            void run() {
                // local queue to swap between producer and consumer to minimize lock time
                std::queue<Msg> local; 

                while (true) {
                    {
                        std::unique_lock<std::mutex> lock(m_mtx);
                        m_cv.wait(lock, [this]() {
                            return m_stopping || !m_queue.empty(); 
                        });
                        if (m_stopping && m_queue.empty()) { break; }
                        // swap contents so we can print without blocking producers
                        local.swap(m_queue); 
                    }

                    while (!local.empty()) {
                        Msg msg = std::move(local.front());
                        local.pop();

                        std::string_view lvl = "UNKN";
                        switch (msg.lvl) {
                            case detail::LogLvl::Debug:   lvl = "DBG"; break;
                            case detail::LogLvl::Info:    lvl = "INF"; break;
                            case detail::LogLvl::Warning: lvl = "WRN"; break;
                            case detail::LogLvl::Error:   lvl = "ERR"; break;
                            case detail::LogLvl::None:    lvl = "NON"; break;
                        }
                        
                        auto ms = static_cast<std::uint64_t>(
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - m_start_time
                            ).count()
                        );

                        auto& out = msg.lvl == detail::LogLvl::Warning || 
                                    msg.lvl == detail::LogLvl::Error
                                    ? std::cerr : std::cout;
                        out << ms << "ms " << lvl << " "<< msg.src << ": " << msg.text << "\n";
                    }

                    std::cout.flush();
                    std::cerr.flush();
                }
            }

            void shutdown() {
                std::cout << "[ LOGGER ] shutting down: " << this << std::endl;
                {
                    std::lock_guard<std::mutex> lock(m_mtx);
                    m_stopping = true;
                }
                m_cv.notify_all();

                if (m_worker.joinable()) {
                    m_worker.join();
                }
            }
    };

    namespace detail {
        logr::Logger& instance() {
            static auto* logger = new Logger();
            return *logger;
        }

        void enqueue(std::string src, LogLvl lvl, std::string msg) {
            auto& logger = instance();
            {
                std::lock_guard<std::mutex> lock(logger.m_mtx);
                if (logger.m_stopping) { return; }
                logger.m_queue.push(
                    Msg{ std::move(src), lvl, std::move(msg) }
                );
            }
            logger.m_cv.notify_one();
        }
    }
}