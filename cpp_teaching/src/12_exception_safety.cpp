/**
 * LESSON 12 — Exception Safety
 * =============================
 * Key ideas:
 *  - What exceptions are and how they propagate (stack unwinding).
 *  - Exception safety guarantees: no-throw, strong, basic.
 *  - noexcept specifier and noexcept operator.
 *  - Writing strongly exception-safe code with copy-and-swap.
 *  - std::exception hierarchy — custom exceptions.
 *  - Common pitfalls: exceptions in destructors, double-fault.
 *  - std::terminate, std::unexpected, std::set_terminate.
 */

#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <utility>
#include <typeinfo>
#include <cassert>
#include <new>          // std::bad_alloc

// ── 1. Exception basics & propagation ────────────────────────────────────────
void exception_basics_demo() {
    std::cout << "\n=== Exception Basics ===\n";

    // throw / try / catch
    auto risky = [](int x) {
        if (x < 0) throw std::invalid_argument("negative value");
        if (x == 0) throw std::domain_error("zero not allowed");
        return 100 / x;
    };

    for (int val : {5, 0, -1}) {
        try {
            int r = risky(val);
            std::cout << "  risky(" << val << ") = " << r << "\n";
        } catch (const std::domain_error& e) {
            std::cout << "  domain_error: " << e.what() << "\n";
        } catch (const std::invalid_argument& e) {
            std::cout << "  invalid_argument: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "  std::exception: " << e.what() << "\n";
        }
        // Note: catch(...) catches everything, but loses type info.
    }

    // Re-throwing with 'throw;' — preserves original type
    auto wrapper = [&]() {
        try {
            risky(-1);
        } catch (...) {
            std::cout << "  wrapper: caught, rethrowing\n";
            throw;              // re-throw same exception object
        }
    };
    try {
        wrapper();
    } catch (const std::exception& e) {
        std::cout << "  outer caught: " << e.what() << "\n";
    }
}

// ── 2. Stack unwinding ────────────────────────────────────────────────────────
struct Resource {
    std::string name;
    explicit Resource(std::string n) : name(std::move(n)) {
        std::cout << "    [" << name << "] acquired\n";
    }
    ~Resource() {
        std::cout << "    [" << name << "] released (unwinding)\n";
    }
};

void inner_fn() {
    Resource r("inner");
    throw std::runtime_error("boom");
}

void outer_fn() {
    Resource r("outer");
    inner_fn();                 // throws — 'outer' still gets destroyed
}

void stack_unwinding_demo() {
    std::cout << "\n=== Stack Unwinding ===\n";
    std::cout << "  Destructors run during unwinding (RAII is safe):\n";
    try {
        outer_fn();
    } catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }
}

// ── 3. Exception safety guarantees ───────────────────────────────────────────
//
//  No-throw guarantee:  the function never throws (mark with noexcept).
//  Strong guarantee:    either succeeds completely OR the state is as if
//                       the function was never called (commit-or-rollback).
//  Basic guarantee:     on exception, the object is in a valid (but possibly
//                       different) state; no resources are leaked.
//  No guarantee:        the function may leave things broken — avoid!

// Strong guarantee via copy-and-swap idiom
class StringVec {
public:
    StringVec() = default;

    // Strong exception-safe assignment via copy-and-swap:
    StringVec& operator=(StringVec other) noexcept {    // copy made in parameter
        swap(*this, other);                              // swap cannot throw
        return *this;
    }

    void push_back(std::string s) {
        // Could throw: vector reallocation, string copy.
        // std::vector already provides strong guarantee for push_back.
        data_.push_back(std::move(s));
    }

    std::size_t size() const noexcept { return data_.size(); }

    friend void swap(StringVec& a, StringVec& b) noexcept {
        using std::swap;
        swap(a.data_, b.data_);
    }

private:
    std::vector<std::string> data_;
};

void exception_safety_demo() {
    std::cout << "\n=== Exception Safety Guarantees ===\n";

    StringVec v;
    v.push_back("hello");
    v.push_back("world");
    std::cout << "  StringVec size = " << v.size() << "\n";

    StringVec v2;
    v2.push_back("foo");
    v = v2;             // copy-and-swap: strongly exception-safe assignment
    std::cout << "  After assignment, v size = " << v.size() << "\n";
}

// ── 4. noexcept specifier and operator ───────────────────────────────────────

// noexcept — promise the compiler this won't throw.
// Enables optimisations (e.g., vector can move instead of copy on reallocate).
int safe_add(int a, int b) noexcept { return a + b; }

// Conditional noexcept — forwards the guarantee of a dependent operation
template<typename T>
void my_swap(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                   std::is_nothrow_move_assignable_v<T>) {
    T tmp = std::move(a);
    a = std::move(b);
    b = std::move(tmp);
}

void noexcept_demo() {
    std::cout << "\n=== noexcept ===\n";

    // noexcept operator: compile-time query
    std::cout << "  safe_add noexcept? " << std::boolalpha
              << noexcept(safe_add(1, 2)) << "\n";

    std::cout << "  int move noexcept? "
              << noexcept(my_swap(*(int*)nullptr, *(int*)nullptr)) << "\n";

    // If a noexcept function throws → std::terminate is called (no catch possible)
    // Never let exceptions escape destructors — mark them noexcept.
    struct SafeDtor {
        ~SafeDtor() noexcept {
            // If cleanup might throw, catch internally:
            try { /* risky cleanup */ }
            catch (...) { /* log but don't rethrow */ }
        }
    };
    std::cout << "  SafeDtor dtor is noexcept\n";
}

// ── 5. Custom exception hierarchy ────────────────────────────────────────────

class AppError : public std::runtime_error {
public:
    explicit AppError(std::string_view msg) : std::runtime_error(std::string(msg)) {}
};

class NetworkError : public AppError {
public:
    int code;
    NetworkError(int code, std::string_view msg)
        : AppError(msg), code(code) {}
};

class DatabaseError : public AppError {
public:
    explicit DatabaseError(std::string_view msg) : AppError(msg) {}
};

void custom_exception_demo() {
    std::cout << "\n=== Custom Exception Hierarchy ===\n";

    auto simulate = [](int scenario) {
        switch (scenario) {
            case 1: throw NetworkError(503, "service unavailable");
            case 2: throw DatabaseError("connection refused");
            case 3: throw AppError("generic app failure");
        }
    };

    for (int s : {1, 2, 3}) {
        try {
            simulate(s);
        } catch (const NetworkError& e) {
            std::cout << "  NetworkError(" << e.code << "): " << e.what() << "\n";
        } catch (const DatabaseError& e) {
            std::cout << "  DatabaseError: " << e.what() << "\n";
        } catch (const AppError& e) {
            std::cout << "  AppError: " << e.what() << "\n";
        }
    }
}

// ── 6. Standard exception types ──────────────────────────────────────────────
void std_exceptions_demo() {
    std::cout << "\n=== Standard Exception Types ===\n";

    // Logic errors — bugs in program logic (should not happen in correct code)
    //   std::logic_error
    //   ├── std::invalid_argument   wrong argument value
    //   ├── std::domain_error       result undefined for input domain
    //   ├── std::length_error       exceeds maximum allowed size
    //   └── std::out_of_range       index/value out of range

    // Runtime errors — errors detected at runtime
    //   std::runtime_error
    //   ├── std::range_error        internal computation range issue
    //   ├── std::overflow_error     arithmetic overflow
    //   └── std::underflow_error    arithmetic underflow

    // Other important ones
    //   std::bad_alloc      — new fails to allocate
    //   std::bad_cast       — dynamic_cast to reference fails
    //   std::bad_typeid     — typeid on null pointer
    //   std::bad_exception  — unexpected exception type

    try {
        std::vector<int> v = {1,2,3};
        v.at(10);                   // throws std::out_of_range
    } catch (const std::out_of_range& e) {
        std::cout << "  out_of_range: " << e.what() << "\n";
    }

    try {
        std::string s;
        s.resize(s.max_size() + 1); // throws std::length_error
    } catch (const std::length_error& e) {
        std::cout << "  length_error: " << e.what() << "\n";
    }

    // bad_alloc
    // try {
    //     auto* p = new int[std::size_t(-1)];  // likely throws bad_alloc
    // } catch (const std::bad_alloc& e) {
    //     std::cout << "  bad_alloc: " << e.what() << "\n";
    // }
}

// ── 7. Exception in constructor / destructor ─────────────────────────────────
void ctor_dtor_exceptions_demo() {
    std::cout << "\n=== Exceptions in Constructors & Destructors ===\n";

    // If a constructor throws:
    //   - Already constructed members are destructed (reverse order).
    //   - The object itself is NOT considered constructed, so ~Foo() won't run.
    //   - Heap memory for 'new Foo' is freed automatically.

    struct Dangerous {
        std::unique_ptr<int> p;
        std::vector<int>     v;

        Dangerous() {
            p = std::make_unique<int>(1);   // acquired
            v.resize(3);                    // acquired
            throw std::runtime_error("ctor failed");
            // p and v destructors run — no leak
        }
    };

    try {
        Dangerous d;
    } catch (const std::exception& e) {
        std::cout << "  ctor threw: " << e.what() << " — no leak (RAII)\n";
    }

    // Destructors MUST NOT throw.
    // If a dtor throws during stack unwinding (another exception active),
    // std::terminate() is called — program dies immediately.
    std::cout << "  Destructors must be noexcept (or swallow exceptions)\n";
}

// ── 8. std::terminate and custom handlers ────────────────────────────────────
void terminate_demo() {
    std::cout << "\n=== std::terminate ===\n";
    std::cout << "  Called when:\n";
    std::cout << "    - exception thrown from noexcept function\n";
    std::cout << "    - exception escapes a thread\n";
    std::cout << "    - double exception during stack unwinding\n";
    std::cout << "    - unhandled exception reaches main\n";
    std::cout << "  Custom handler: std::set_terminate([]{ /* log */ std::abort(); });\n";
}

int main() {
    exception_basics_demo();
    stack_unwinding_demo();
    exception_safety_demo();
    noexcept_demo();
    custom_exception_demo();
    std_exceptions_demo();
    ctor_dtor_exceptions_demo();
    terminate_demo();

    std::cout << "\n✓ Lesson 12 complete.\n";
}
