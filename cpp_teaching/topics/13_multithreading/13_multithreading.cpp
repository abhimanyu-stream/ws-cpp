/**
 * TOPIC 13 — Multithreading and Concurrency
 * ===========================================
 * Covers: std::thread, mutex, lock_guard, condition_variable,
 *         atomic, memory ordering, futures/promises, async,
 *         race conditions, deadlocks, producer-consumer
 *
 * Compile: g++ -std=c++17 -Wall -Wno-unused-parameter -pthread 13_multithreading.cpp -o threading
 *
 * NOTE: -pthread is required on Linux/Cygwin to link the POSIX threads library.
 *       On Windows with MSVC, threading is built-in (no extra flag needed).
 *       On Cygwin64: E:/Installation/CTools/cygwin64/bin/g++ -std=c++17 -pthread ...
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <algorithm>
#include <numeric>

using namespace std::chrono_literals;

// ── Thread Creation ───────────────────────────────────────────────────────────
void section_thread_basics() {
    std::cout << "\n=== Thread Creation ===\n";

    // Thread with function
    auto task = [](int id, int n) {
        std::cout << "Thread " << id << " counting to " << n << "\n";
    };

    std::thread t1(task, 1, 5);
    std::thread t2(task, 2, 3);

    // join() — wait for thread to finish before continuing
    t1.join();
    t2.join();
    std::cout << "Both threads done\n";

    // detach() — thread runs independently (daemon-style)
    // std::thread t3(task, 3, 2);
    // t3.detach();   // t3 runs independently — don't join or destroy it
    // WARNING: if detached thread outlives main, program terminates → UB

    // Thread ID and hardware concurrency
    std::cout << "main thread id:        " << std::this_thread::get_id() << "\n";
    std::cout << "hardware_concurrency:  " << std::thread::hardware_concurrency() << "\n";
}

// ── Race Condition ─────────────────────────────────────────────────────────────
// Two threads modifying the same variable without synchronization.
int unsafe_counter = 0;

void section_race_condition() {
    std::cout << "\n=== Race Condition Demo ===\n";

    // BAD — data race: undefined behavior!
    auto unsafe_increment = []() {
        for (int i = 0; i < 10000; ++i)
            ++unsafe_counter;    // read-modify-write is NOT atomic
    };

    std::thread t1(unsafe_increment);
    std::thread t2(unsafe_increment);
    t1.join(); t2.join();

    std::cout << "unsafe_counter (expected 20000): " << unsafe_counter
              << " (may differ — race condition!)\n";
}

// ── Mutex ─────────────────────────────────────────────────────────────────────
// Mutual exclusion — only one thread can hold the lock at a time.
std::mutex       g_mutex;
int              safe_counter = 0;

void section_mutex() {
    std::cout << "\n=== Mutex ===\n";

    // Manual lock/unlock (error-prone — always prefer RAII guards)
    auto manual_increment = []() {
        for (int i = 0; i < 10000; ++i) {
            g_mutex.lock();
            ++safe_counter;
            g_mutex.unlock();
        }
    };

    // RAII lock_guard — automatically unlocks when destroyed (scope ends or exception)
    auto safe_increment = []() {
        for (int i = 0; i < 10000; ++i) {
            std::lock_guard<std::mutex> lock(g_mutex);
            ++safe_counter;
        }   // lock released here automatically
    };

    std::thread t1(safe_increment);
    std::thread t2(safe_increment);
    t1.join(); t2.join();

    std::cout << "safe_counter (expected 20000): " << safe_counter << "\n";

    // unique_lock — more flexible than lock_guard: can unlock/relock, deferred lock
    std::unique_lock<std::mutex> ulock(g_mutex, std::defer_lock);
    ulock.lock();     // manually lock
    ++safe_counter;
    ulock.unlock();   // manually unlock (before scope ends)

    // shared_mutex (C++14) — multiple readers OR one writer
    std::shared_mutex rw_mutex;
    // Multiple readers simultaneously:
    // std::shared_lock<std::shared_mutex> read_lock(rw_mutex);
    // Single writer:
    // std::unique_lock<std::shared_mutex> write_lock(rw_mutex);
}

// ── Condition Variables ───────────────────────────────────────────────────────
// Used to block a thread until a condition is true.
std::mutex              cv_mutex;
std::condition_variable cv;
bool                    data_ready = false;
int                     shared_data = 0;

void section_condition_variable() {
    std::cout << "\n=== Condition Variable ===\n";

    // Producer thread
    auto producer = []() {
        std::this_thread::sleep_for(50ms);  // simulate work
        {
            std::lock_guard<std::mutex> lock(cv_mutex);
            shared_data  = 42;
            data_ready   = true;
        }
        cv.notify_one();   // wake up one waiting thread
        std::cout << "Producer: data=" << shared_data << " ready\n";
    };

    // Consumer thread
    auto consumer = []() {
        std::unique_lock<std::mutex> lock(cv_mutex);
        // wait() atomically: releases lock, blocks, re-acquires lock when notified
        // The lambda is the predicate — guards against spurious wakeups
        cv.wait(lock, []{ return data_ready; });
        std::cout << "Consumer: received data=" << shared_data << "\n";
    };

    std::thread prod(producer);
    std::thread cons(consumer);
    prod.join();
    cons.join();
}

// ── Atomic Variables ──────────────────────────────────────────────────────────
// Lock-free, thread-safe operations on individual values.
std::atomic<int>  atomic_counter{0};
std::atomic<bool> stop_flag{false};

void section_atomic() {
    std::cout << "\n=== Atomic Variables ===\n";

    auto increment = []() {
        for (int i = 0; i < 10000; ++i)
            ++atomic_counter;    // atomic increment — no mutex needed
    };

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join(); t2.join();

    std::cout << "atomic_counter (expected 20000): " << atomic_counter.load() << "\n";

    // Compare-and-swap — foundation of lock-free algorithms
    int expected = atomic_counter.load();
    bool swapped = atomic_counter.compare_exchange_strong(expected, 0);
    std::cout << "CAS reset to 0: " << swapped
              << " new value: " << atomic_counter.load() << "\n";

    // Memory ordering (brief overview):
    // seq_cst  — total order, most restrictions (default)
    // acquire  — reads cannot move before this
    // release  — writes cannot move after this
    // relaxed  — no ordering constraints (fastest, most dangerous)
    atomic_counter.store(100, std::memory_order_relaxed);
    int val = atomic_counter.load(std::memory_order_relaxed);
    std::cout << "relaxed load: " << val << "\n";
}

// ── Futures, Promises, std::async ────────────────────────────────────────────
int heavy_computation(int n) {
    std::this_thread::sleep_for(50ms);   // simulate work
    return n * n;
}

void section_futures() {
    std::cout << "\n=== Futures & Promises ===\n";

    // std::async — run function asynchronously, get result via future
    auto fut1 = std::async(std::launch::async, heavy_computation, 7);
    auto fut2 = std::async(std::launch::async, heavy_computation, 8);

    // Do other work here while computations run in parallel...

    int r1 = fut1.get();   // blocks until result is available
    int r2 = fut2.get();
    std::cout << "async results: " << r1 << " " << r2 << "\n";

    // Promise — explicitly set a value that a future will receive
    std::promise<int> promise;
    std::future<int>  future = promise.get_future();

    std::thread worker([&promise]() {
        std::this_thread::sleep_for(30ms);
        promise.set_value(999);   // fulfill the promise
    });

    std::cout << "waiting for promise...\n";
    std::cout << "promise value: " << future.get() << "\n";
    worker.join();

    // packaged_task — wrap a callable for async execution
    std::packaged_task<int(int)> task(heavy_computation);
    std::future<int> task_fut = task.get_future();
    std::thread task_thread(std::move(task), 10);
    task_thread.join();
    std::cout << "packaged_task result: " << task_fut.get() << "\n";
}

// ── Producer-Consumer Pattern ─────────────────────────────────────────────────
template<typename T>
class ThreadSafeQueue {
public:
    void push(T val) {
        {
            std::lock_guard<std::mutex> lock(m_);
            q_.push(std::move(val));
        }
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [this]{ return !q_.empty() || done_; });
        if (q_.empty()) throw std::runtime_error("queue closed");
        T val = std::move(q_.front());
        q_.pop();
        return val;
    }

    void done() {
        done_ = true;
        cv_.notify_all();
    }

private:
    std::queue<T>           q_;
    std::mutex              m_;
    std::condition_variable cv_;
    std::atomic<bool>       done_{false};
};

void section_producer_consumer() {
    std::cout << "\n=== Producer-Consumer ===\n";

    ThreadSafeQueue<int> q;

    // Producer
    std::thread prod([&q]() {
        for (int i = 1; i <= 5; ++i) {
            q.push(i);
            std::cout << "  produced: " << i << "\n";
            std::this_thread::sleep_for(10ms);
        }
        q.done();
    });

    // Consumer
    std::thread cons([&q]() {
        try {
            while (true) {
                int val = q.pop();
                std::cout << "  consumed: " << val << "\n";
            }
        }
        catch (const std::runtime_error&) {
            std::cout << "  consumer: queue closed\n";
        }
    });

    prod.join();
    cons.join();
}

// ── Deadlock Prevention ───────────────────────────────────────────────────────
void section_deadlock() {
    std::cout << "\n=== Deadlock Prevention ===\n";

    std::mutex m1, m2;

    // DEADLOCK scenario (don't run):
    // Thread 1: lock(m1) then lock(m2)
    // Thread 2: lock(m2) then lock(m1)
    // Both wait for the other forever

    // SOLUTION 1: std::lock — acquires both atomically
    auto safe_transfer = [&]() {
        std::lock(m1, m2);   // locks both atomically — no deadlock
        std::lock_guard<std::mutex> lg1(m1, std::adopt_lock);
        std::lock_guard<std::mutex> lg2(m2, std::adopt_lock);
        std::cout << "  both locks acquired safely\n";
    };

    // SOLUTION 2: std::scoped_lock (C++17) — cleaner
    auto safe_transfer2 = [&]() {
        std::scoped_lock lock(m1, m2);  // acquires both, RAII, no deadlock
        std::cout << "  scoped_lock: both acquired\n";
    };

    safe_transfer();
    safe_transfer2();
}

int main() {
    std::cout << "╔══ Topic 13: Multithreading ══╗\n";

    section_thread_basics();
    section_race_condition();
    section_mutex();
    section_condition_variable();
    section_atomic();
    section_futures();
    section_producer_consumer();
    section_deadlock();

    std::cout << "\n✓ Topic 13 complete.\n";
}
