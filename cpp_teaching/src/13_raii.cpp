/**
 * LESSON 13 — RAII (Resource Acquisition Is Initialization)
 * ===========================================================
 * Key ideas:
 *  - What RAII is and why it matters.
 *  - Tying resource lifetime to object lifetime.
 *  - Writing RAII wrappers: file handle, mutex guard, timer.
 *  - The Rule of Five and the Rule of Zero.
 *  - std::unique_ptr / std::shared_ptr as RAII smart pointers (recap).
 *  - Scope guards — generic RAII for arbitrary cleanup.
 *  - RAII vs. try/finally (why RAII wins in C++).
 */

#include <iostream>
#include <fstream>
#include <mutex>
#include <memory>
#include <functional>
#include <chrono>
#include <string>
#include <string_view>
#include <stdexcept>
#include <utility>
#include <cassert>

using namespace std::chrono_literals;

// ── 1. What is RAII? ─────────────────────────────────────────────────────────
//
//  Principle: bind the lifetime of a resource (memory, file, socket, lock, ...)
//  to the lifetime of an object on the stack.
//
//    Constructor  → acquire resource
//    Destructor   → release resource  (runs automatically, even on exceptions)
//
//  This guarantees resources are always released — no manual cleanup, no leaks.

void raii_intro_demo() {
    std::cout << "\n=== RAII — Core Idea ===\n";

    // std::ifstream is a classic RAII type: opens file in ctor, closes in dtor.
    {
        std::ofstream out("raii_test.txt");   // create a temp file
        out << "RAII test\n";
    }   // <-- file closed here automatically

    {
        std::ifstream in("raii_test.txt");    // open
        if (in) {
            std::string line;
            std::getline(in, line);
            std::cout << "  read: " << line << "\n";
        }
    }   // <-- closed here, even if an exception was thrown above

    std::remove("raii_test.txt");
    std::cout << "  File opened and closed via RAII (ifstream)\n";
}

// ── 2. Custom RAII file wrapper ───────────────────────────────────────────────
class FileHandle {
public:
    explicit FileHandle(const char* path, const char* mode) {
        file_ = std::fopen(path, mode);
        if (!file_) throw std::runtime_error(std::string("cannot open ") + path);
        std::cout << "  FileHandle: opened '" << path << "'\n";
    }

    // No copy — a file handle is a unique resource
    FileHandle(const FileHandle&)            = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Move is fine
    FileHandle(FileHandle&& other) noexcept : file_(other.file_) {
        other.file_ = nullptr;
    }
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            if (file_) std::fclose(file_);
            file_ = other.file_;
            other.file_ = nullptr;
        }
        return *this;
    }

    ~FileHandle() {
        if (file_) {
            std::fclose(file_);
            std::cout << "  FileHandle: closed\n";
        }
    }

    std::FILE* get() const noexcept { return file_; }

    void write(std::string_view s) {
        std::fwrite(s.data(), 1, s.size(), file_);
    }

private:
    std::FILE* file_ = nullptr;
};

void file_handle_demo() {
    std::cout << "\n=== Custom RAII FileHandle ===\n";

    try {
        FileHandle f("raii_demo.txt", "w");
        f.write("hello from RAII\n");
        // f closes automatically here (or even if an exception is thrown)
    } catch (const std::exception& e) {
        std::cout << "  error: " << e.what() << "\n";
    }

    // Try to open a non-existent path for reading
    try {
        FileHandle bad("/no/such/path/file.txt", "r");
    } catch (const std::exception& e) {
        std::cout << "  expected error: " << e.what() << "\n";
    }

    std::remove("raii_demo.txt");
}

// ── 3. RAII mutex lock ────────────────────────────────────────────────────────
// (std::lock_guard already does this, shown here as a teaching example)

class MutexLock {
public:
    explicit MutexLock(std::mutex& m) : mtx_(m) {
        mtx_.lock();
        std::cout << "  MutexLock: acquired\n";
    }
    ~MutexLock() {
        mtx_.unlock();
        std::cout << "  MutexLock: released\n";
    }
    MutexLock(const MutexLock&)            = delete;
    MutexLock& operator=(const MutexLock&) = delete;

private:
    std::mutex& mtx_;
};

void mutex_raii_demo() {
    std::cout << "\n=== RAII Mutex ===\n";
    std::mutex m;
    {
        MutexLock lock(m);
        std::cout << "  critical section\n";
    }   // unlocked automatically
}

// ── 4. RAII timer ────────────────────────────────────────────────────────────
class ScopedTimer {
public:
    explicit ScopedTimer(std::string_view label)
        : label_(label), start_(std::chrono::high_resolution_clock::now()) {
        std::cout << "  [timer '" << label_ << "' started]\n";
    }
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto us  = std::chrono::duration_cast<std::chrono::microseconds>(end - start_).count();
        std::cout << "  [timer '" << label_ << "' elapsed: " << us << " µs]\n";
    }
    ScopedTimer(const ScopedTimer&)            = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    std::string label_;
    std::chrono::high_resolution_clock::time_point start_;
};

void timer_demo() {
    std::cout << "\n=== RAII Timer ===\n";
    {
        ScopedTimer t("work block");
        volatile long long sum = 0;
        for (int i = 0; i < 1'000'000; ++i) sum += i;
        std::cout << "  sum = " << sum << "\n";
    }   // timer prints elapsed time here
}

// ── 5. Rule of Five / Rule of Zero ───────────────────────────────────────────
//
//  Rule of Five:
//    If you define ANY of these, define all five:
//      1. Destructor
//      2. Copy constructor
//      3. Copy assignment operator
//      4. Move constructor
//      5. Move assignment operator
//
//  Rule of Zero:
//    Prefer to own resources through RAII members (unique_ptr, vector, string).
//    Then all five special members can be =default or compiler-generated.
//    This is almost always the right approach in modern C++.

// Rule of Zero example — compiler generates everything correctly
struct Employee {
    std::string              name;      // RAII string
    std::unique_ptr<int[]>   data;      // RAII smart pointer
    std::vector<std::string> history;   // RAII container

    // No destructor, no copy/move — compiler does the right thing.
    Employee(std::string n, int data_size)
        : name(std::move(n)), data(std::make_unique<int[]>(data_size)) {}
};

void rule_of_five_demo() {
    std::cout << "\n=== Rule of Five / Rule of Zero ===\n";

    Employee e1("Alice", 10);
    // e1 is non-copyable because unique_ptr is non-copyable — compiler enforces it.

    Employee e2 = std::move(e1);    // move is fine
    std::cout << "  moved employee name: " << e2.name << "\n";
    std::cout << "  Rule of Zero: compiler generated correct move semantics\n";
}

// ── 6. Scope guard — generic RAII cleanup ────────────────────────────────────
// A scope guard runs arbitrary cleanup code when it goes out of scope.
// Useful when you can't write a dedicated RAII class.

class ScopeGuard {
public:
    explicit ScopeGuard(std::function<void()> cleanup)
        : cleanup_(std::move(cleanup)), active_(true) {}

    // Allow disarming — skip cleanup if everything went fine
    void disarm() noexcept { active_ = false; }

    ~ScopeGuard() {
        if (active_) {
            try { cleanup_(); }
            catch (...) {}   // never let dtor throw
        }
    }

    ScopeGuard(const ScopeGuard&)            = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    std::function<void()> cleanup_;
    bool                  active_;
};

void scope_guard_demo() {
    std::cout << "\n=== Scope Guard ===\n";

    // Always runs cleanup:
    {
        std::cout << "  acquiring resource\n";
        ScopeGuard guard([]{ std::cout << "  scope guard: cleanup ran\n"; });
        std::cout << "  using resource\n";
    }   // cleanup runs here

    // Disarmed — cleanup skipped on success:
    {
        std::cout << "  starting transaction\n";
        ScopeGuard rollback([]{ std::cout << "  scope guard: rolling back!\n"; });
        // ... do work ...
        std::cout << "  work succeeded — disarming rollback guard\n";
        rollback.disarm();
    }   // no rollback

    // Triggered on exception:
    std::cout << "  starting risky operation\n";
    try {
        ScopeGuard guard([]{ std::cout << "  scope guard: emergency cleanup\n"; });
        throw std::runtime_error("something went wrong");
        guard.disarm();     // never reached
    } catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }
}

// ── 7. RAII vs try/finally ────────────────────────────────────────────────────
void raii_vs_finally_demo() {
    std::cout << "\n=== RAII vs try/finally ===\n";

    // Languages like Java/Python use try/finally for cleanup:
    //
    //   resource = acquire()
    //   try:
    //       use(resource)
    //   finally:
    //       release(resource)       <-- must be written every time
    //
    // Problems:
    //   - Easy to forget
    //   - Duplicates cleanup code
    //   - Nested resources get messy
    //   - Early returns bypass the finally block in some languages
    //
    // C++ RAII:
    //   - Cleanup is in the destructor — written once, never forgotten
    //   - Works with early returns, exceptions, and any exit path
    //   - Composable: destructors chain automatically

    std::cout << "  RAII advantages over try/finally:\n";
    std::cout << "    - Cleanup defined once in dtor, not at every call site\n";
    std::cout << "    - Works for all exit paths: return, throw, end-of-scope\n";
    std::cout << "    - Composable: member dtors chain automatically\n";
    std::cout << "    - Compiler enforces it — cannot be accidentally skipped\n";
}

// ── 8. RAII in the standard library ──────────────────────────────────────────
void stdlib_raii_demo() {
    std::cout << "\n=== RAII in the Standard Library ===\n";

    // Memory
    {
        auto p = std::make_unique<int>(42);     // unique_ptr: sole ownership
        auto q = std::make_shared<int>(99);     // shared_ptr: shared ownership
        std::cout << "  unique_ptr: " << *p << "  shared_ptr: " << *q << "\n";
    }   // both freed here

    // Streams
    {
        std::ofstream f("tmp_raii.txt");
        f << "test";
    }   // flushed and closed
    std::remove("tmp_raii.txt");

    // Containers (own their heap memory)
    {
        std::vector<int> v(1'000'000, 0);       // 4 MB on heap
        v[0] = 1;
    }   // freed here

    // Locks
    {
        std::mutex m;
        std::lock_guard lock(m);                // locked
        // critical section
    }   // unlocked

    std::cout << "  string, vector, fstream, lock_guard, unique_ptr, shared_ptr\n";
    std::cout << "  — all standard RAII types\n";
}

int main() {
    raii_intro_demo();
    file_handle_demo();
    mutex_raii_demo();
    timer_demo();
    rule_of_five_demo();
    scope_guard_demo();
    raii_vs_finally_demo();
    stdlib_raii_demo();

    std::cout << "\n✓ Lesson 13 complete.\n";
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "  All 13 lessons complete! 🎉\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
}
