#include <condition_variable>
#include <mutex>
#include <queue>

namespace evt {
    // releases wait if stop() or post() is called
    class Signal {
    private:
        std::mutex mtx;
        std::condition_variable cv;
        int permits = 0;
        bool stopping = false;

    public:
        void post() {
            {
                std::lock_guard lock(mtx);
                ++permits;
            }
            cv.notify_one();
        }

        void stop() {
            {
                std::lock_guard lock(mtx);
                stopping = true;
            }
            cv.notify_all();
        }

        bool wait_one() {
            std::unique_lock lock(mtx);

            cv.wait(lock, [&] {
                return permits > 0 || stopping;
            });

            if (stopping) {
                return false;
            }

            --permits;
            return true;
        }
    };
}