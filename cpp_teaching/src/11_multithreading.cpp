/**
 * LESSON 11 — Multithreading
 * ===========================
 * Key ideas:
 *  - std::thread: creation, join, detach.
 *  - Data races and undefined behaviour.
 *  - std::mutex, std::lock_guard, std::unique_lock.
 *  - std::condition_variable — producer/consumer.
 *  - std::atomic — lock-free primitives.
 *  - std::async / std::future / std::promise.
 *  - std::call_once — thread-safe one-time init.
 *  - Thread-local storage.
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <vector>
#include <queue>
#include <string>
#include <string_view>
#include <chrono>
#include <numeric>
#include <cassert>

using namespace std::chrono_literals;   // 100ms, 1s ...

// ── 1. Basic thread creation ─────────────────────────────────────────────────
void thread_basics_demo() {
    std::cout << "\n=== Thread Basics ===\n";

    // Thread with a lambda
    std::thread t1([]{ std::cout << "  t1: hello from thread\n"; });
    t1.join();                          // wait for t1 to finish

    // Thread with arguments
    auto work = [](int id, std::string_view msg) {
        std::cout << "  t" << id << ": " << msg << "\n";
    };
    std::thread t2(work, 2, "doing work");
    std::thread t3(work, 3, "also working");
    t2.join();
    t3.join();

    // hardware_concurrency — hint at the number of hardware threads
    std::cout << "  hardware_concurrency = "
              << std::thread::hardware_concurrency() << "\n";

    // thread::id
    std::thread t4([]{
        std::cout << "  t4 id = " << std::this_thread::get_id() << "\n";
    });
    std::cout << "  main id = " << std::this_thread::get_id() << "\n";
    t4.join();
}

// ── 2. Data race (the problem) ────────────────────────────────────────────────
void data_race_demo() {
    std::cout << "\n=== Data Race (demonstrated safely) ===\n";

    // WITHOUT synchronisation this is a data race (UB).
    // We demonstrate the PATTERN, then show the fix.
    //
    // int counter = 0;                      // <-- shared, unprotected
    // std::thread t1([&]{ for(int i=0;i<100000;++i) ++counter; });
    // std::thread t2([&]{ for(int i=0;i<100000;++i) ++counter; });
    // t1.join(); t2.join();
    // // counter is probably NOT 200000 — race condition!
    //
    std::cout << "  (data race code is commented out — it is UB)\n";
    std::cout << "  Fix: protect shared state with a mutex or atomic.\n";
}

// ── 3. std::mutex and lock guards ────────────────────────────────────────────
void mutex_demo() {
    std::cout << "\n=== Mutex & Lock Guards ===\n";

    int counter = 0;
    std::mutex mtx;

    auto increment = [&](int times) {
        for (int i = 0; i < times; ++i) {
            std::lock_guard<std::mutex> lock(mtx);  // RAII: locks on construction, unlocks on destruction
            ++counter;
        }
    };

    std::thread t1(increment, 100'000);
    std::thread t2(increment, 100'000);
    t1.join();
    t2.join();

    std::cout << "  counter (expected 200000) = " << counter << "\n";
    assert(counter == 200'000);

    // std::unique_lock — more flexible than lock_guard:
    // can unlock/relock, used with condition_variable, supports try_lock
    std::mutex mtx2;
    {
        std::unique_lock<std::mutex> lock(mtx2);
        // critical section
        lock.unlock();              // unlock early
        // non-critical work ...
        lock.lock();                // re-lock
        // critical section again
    }   // lock destroyed, automatically unlocked
    std::cout << "  unique_lock demonstrated\n";
}

// ── 4. Deadlock and std::scoped_lock (C++17) ─────────────────────────────────
void scoped_lock_demo() {
    std::cout << "\n=== Avoiding Deadlock with scoped_lock ===\n";

    std::mutex m1, m2;

    // Classic deadlock: thread A locks m1 then m2, thread B locks m2 then m1.
    // Fix: always lock multiple mutexes together atomically.

    auto task = [&](std::string_view name, int v1, int v2) {
        // scoped_lock acquires all mutexes in a deadlock-free order
        std::scoped_lock lock(m1, m2);    // C++17
        std::cout << "  " << name << " holds both locks: "
                  << v1 << " + " << v2 << " = " << (v1 + v2) << "\n";
    };

    std::thread t1(task, "thread-A", 1, 2);
    std::thread t2(task, "thread-B", 3, 4);
    t1.join();
    t2.join();
}

// ── 5. condition_variable — producer/consumer ────────────────────────────────
void condition_variable_demo() {
    std::cout << "\n=== condition_variable — Producer/Consumer ===\n";

    std::mutex              mtx;
    std::condition_variable cv;
    std::queue<int>         q;
    bool                    done = false;

    // Producer
    std::thread producer([&]{
        for (int i = 1; i <= 5; ++i) {
            std::this_thread::sleep_for(10ms);
            {
                std::lock_guard lock(mtx);
                q.push(i);
                std::cout << "  produced: " << i << "\n";
            }
            cv.notify_one();            // wake up one waiting consumer
        }
        {
            std::lock_guard lock(mtx);
            done = true;
        }
        cv.notify_all();
    });

    // Consumer
    std::thread consumer([&]{
        while (true) {
            std::unique_lock lock(mtx);
            cv.wait(lock, [&]{ return !q.empty() || done; });  // spurious-wake safe
            while (!q.empty()) {
                std::cout << "  consumed: " << q.front() << "\n";
                q.pop();
            }
            if (done && q.empty()) break;
        }
    });

    producer.join();
    consumer.join();
}

// ── 6. std::atomic ────────────────────────────────────────────────────────────
void atomic_demo() {
    std::cout << "\n=== std::atomic ===\n";

    std::atomic<int> counter{0};

    auto increment = [&](int times) {
        for (int i = 0; i < times; ++i)
            counter.fetch_add(1, std::memory_order_relaxed);  // lock-free increment
    };

    std::thread t1(increment, 100'000);
    std::thread t2(increment, 100'000);
    t1.join();
    t2.join();

    std::cout << "  atomic counter (expected 200000) = " << counter.load() << "\n";
    assert(counter == 200'000);

    // CAS — compare-and-swap: the foundation of lock-free data structures
    std::atomic<int> val{10};
    int expected = 10;
    bool swapped = val.compare_exchange_strong(expected, 20);
    std::cout << "  CAS(10 → 20): swapped=" << std::boolalpha << swapped
              << "  val=" << val.load() << "\n";

    // atomic flag — the lightest atomic type (guaranteed lock-free)
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    std::cout << "  atomic_flag test_and_set: " << flag.test_and_set() << "\n";  // 0 = was clear
    std::cout << "  atomic_flag test_and_set: " << flag.test_and_set() << "\n";  // 1 = was set
    flag.clear();
}

// ── 7. std::async / std::future ───────────────────────────────────────────────
long long parallel_sum(const std::vector<int>& v, std::size_t lo, std::size_t hi) {
    long long s = 0;
    for (std::size_t i = lo; i < hi; ++i) s += v[i];
    return s;
}

void async_future_demo() {
    std::cout << "\n=== std::async & std::future ===\n";

    // Simple async task
    auto future = std::async(std::launch::async, []{
        std::this_thread::sleep_for(20ms);
        return 42;
    });
    std::cout << "  async result = " << future.get() << "\n";  // blocks until ready

    // Parallel sum using futures
    std::vector<int> v(1'000'000);
    std::iota(v.begin(), v.end(), 1);
    std::size_t mid = v.size() / 2;

    auto f1 = std::async(std::launch::async, parallel_sum, std::cref(v), 0, mid);
    auto f2 = std::async(std::launch::async, parallel_sum, std::cref(v), mid, v.size());
    long long total = f1.get() + f2.get();
    std::cout << "  parallel sum 1..1M = " << total << "\n";

    // std::launch::deferred — runs lazily in the calling thread when .get() is called
    auto lazy = std::async(std::launch::deferred, []{ return "lazy evaluated"; });
    std::cout << "  deferred = " << lazy.get() << "\n";
}

// ── 8. std::promise / std::future pair ───────────────────────────────────────
void promise_demo() {
    std::cout << "\n=== std::promise / std::future ===\n";

    std::promise<int> promise;
    auto future = promise.get_future();   // shared state between promise and future

    std::thread worker([](std::promise<int> p){
        std::this_thread::sleep_for(10ms);
        p.set_value(1234);               // fulfil the promise
    }, std::move(promise));

    std::cout << "  promise result = " << future.get() << "\n";
    worker.join();

    // Exceptions can also be communicated:
    std::promise<int> err_promise;
    auto err_future = err_promise.get_future();
    std::thread err_worker([](std::promise<int> p){
        try {
            throw std::runtime_error("worker error");
        } catch (...) {
            p.set_exception(std::current_exception());
        }
    }, std::move(err_promise));
    err_worker.join();
    try {
        err_future.get();
    } catch (const std::exception& e) {
        std::cout << "  caught exception from worker: " << e.what() << "\n";
    }
}

// ── 9. std::call_once — thread-safe singleton init ───────────────────────────
struct Singleton {
    static Singleton& instance() {
        static std::once_flag flag;
        static std::unique_ptr<Singleton> inst;
        std::call_once(flag, []{
            inst = std::make_unique<Singleton>();
            std::cout << "  Singleton created (once)\n";
        });
        return *inst;
    }
    int value = 42;
private:
    Singleton() = default;
};

void call_once_demo() {
    std::cout << "\n=== std::call_once ===\n";

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
        threads.emplace_back([]{ Singleton::instance(); });
    for (auto& t : threads) t.join();
    std::cout << "  value = " << Singleton::instance().value << "\n";
}

// ── 10. Thread-local storage ─────────────────────────────────────────────────
thread_local int tls_counter = 0;   // each thread has its own copy

void thread_local_demo() {
    std::cout << "\n=== Thread-Local Storage ===\n";

    auto work = [](int id) {
        for (int i = 0; i < 5; ++i) ++tls_counter;
        std::cout << "  thread " << id << " tls_counter = " << tls_counter << "\n";
    };

    std::thread t1(work, 1);
    std::thread t2(work, 2);
    t1.join();
    t2.join();
    std::cout << "  main tls_counter = " << tls_counter << "\n";  // 0, unmodified
}

int main() {
    thread_basics_demo();
    data_race_demo();
    mutex_demo();
    scoped_lock_demo();
    condition_variable_demo();
    atomic_demo();
    async_future_demo();
    promise_demo();
    call_once_demo();
    thread_local_demo();

    std::cout << "\n✓ Lesson 11 complete.\n";
}
