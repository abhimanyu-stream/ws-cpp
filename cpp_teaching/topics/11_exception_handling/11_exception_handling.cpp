/**
 * TOPIC 11 — Exception Handling
 * ===============================
 * Covers: try/catch/throw, multiple catch, catch-all, custom exceptions,
 *         propagation, stack unwinding, noexcept, exception safety,
 *         RAII and exceptions, standard exception hierarchy
 *
 * Compile: g++ -std=c++17 -Wall 11_exception_handling.cpp -o exceptions
 */

#include <iostream>
#include <string>
#include <stdexcept>   // standard exception classes
#include <memory>
#include <vector>
#include <fstream>

// ── try / catch / throw ───────────────────────────────────────────────────────
void section_basic() {
    std::cout << "\n=== try / catch / throw ===\n";

    // throw an exception — immediately exits the current function and
    // unwinds the call stack until a matching catch is found
    auto risky = [](int x) {
        if (x == 0) throw std::runtime_error("division by zero");
        return 100 / x;
    };

    try {
        std::cout << risky(5)  << "\n";   // OK: 20
        std::cout << risky(0)  << "\n";   // throws — this line never reached
        std::cout << risky(2)  << "\n";   // never reached
    }
    catch (const std::runtime_error& e) {
        // e.what() — the error message
        std::cout << "Caught runtime_error: " << e.what() << "\n";
    }
    std::cout << "Execution continues after try-catch\n";
}

// ── Multiple catch blocks ─────────────────────────────────────────────────────
void section_multiple_catch() {
    std::cout << "\n=== Multiple catch ===\n";

    auto dangerous = [](int choice) {
        switch (choice) {
            case 1: throw std::invalid_argument("bad argument");
            case 2: throw std::out_of_range("index out of range");
            case 3: throw std::runtime_error("runtime failure");
            case 4: throw 42;              // throwing an int (not recommended)
            case 5: throw "raw string";    // throwing const char* (not recommended)
        }
    };

    for (int i = 1; i <= 5; ++i) {
        try {
            dangerous(i);
        }
        catch (const std::invalid_argument& e) { std::cout << "invalid_arg: " << e.what() << "\n"; }
        catch (const std::out_of_range& e)      { std::cout << "out_of_range: " << e.what() << "\n"; }
        catch (const std::runtime_error& e)     { std::cout << "runtime_err: "  << e.what() << "\n"; }
        catch (int n)                           { std::cout << "int thrown: "    << n        << "\n"; }
        catch (const char* s)                   { std::cout << "cstr thrown: "   << s        << "\n"; }
        catch (...) {
            // Catch-all — matches anything
            std::cout << "unknown exception\n";
        }
    }
}

// ── Custom Exception Classes ──────────────────────────────────────────────────
// Best practice: derive from std::exception or one of its subclasses.
class DatabaseError : public std::runtime_error {
public:
    explicit DatabaseError(const std::string& msg, int code = -1)
        : std::runtime_error(msg), error_code_(code) {}

    int error_code() const { return error_code_; }

    // what() is inherited from std::runtime_error
private:
    int error_code_;
};

class ConnectionError : public DatabaseError {
public:
    explicit ConnectionError(const std::string& host)
        : DatabaseError("Cannot connect to: " + host, 503)
        , host_(host) {}

    const std::string& host() const { return host_; }
private:
    std::string host_;
};

void section_custom_exceptions() {
    std::cout << "\n=== Custom Exceptions ===\n";

    try {
        throw ConnectionError("db.example.com");
    }
    catch (const ConnectionError& e) {
        // Most-derived class caught first — always order from specific to general
        std::cout << "ConnectionError: " << e.what()
                  << " (code=" << e.error_code()
                  << " host=" << e.host() << ")\n";
    }
    catch (const DatabaseError& e) {
        std::cout << "DatabaseError: " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "std::exception: " << e.what() << "\n";
    }
}

// ── Exception Propagation ─────────────────────────────────────────────────────
// Exceptions propagate UP the call stack until caught.
void level3() { throw std::runtime_error("from level3"); }
void level2() { level3(); }  // not caught here — propagates up
void level1() { level2(); }  // not caught here — propagates up

void section_propagation() {
    std::cout << "\n=== Exception Propagation ===\n";
    try {
        level1();   // calls level2 → level3 → throws
    }
    catch (const std::exception& e) {
        std::cout << "Caught at top level: " << e.what() << "\n";
    }

    // Re-throw — catch, do something, then re-throw same exception
    try {
        try {
            throw std::runtime_error("original");
        }
        catch (const std::exception& e) {
            std::cout << "Inner catch (logging): " << e.what() << "\n";
            throw;   // re-throw same exception object (not a copy)
        }
    }
    catch (const std::exception& e) {
        std::cout << "Outer catch: " << e.what() << "\n";
    }
}

// ── Stack Unwinding ───────────────────────────────────────────────────────────
// When an exception is thrown, C++ destroys all local objects in reverse order.
// This is why RAII works — destructors always run.
struct Tracked {
    std::string name;
    explicit Tracked(std::string n) : name(std::move(n)) {
        std::cout << "  Tracked('" << name << "') created\n";
    }
    ~Tracked() {
        std::cout << "  Tracked('" << name << "') destroyed (stack unwind)\n";
    }
};

void might_throw(bool do_throw) {
    Tracked a("first");
    Tracked b("second");
    if (do_throw) throw std::runtime_error("thrown!");
    Tracked c("third");   // only reached if no throw
}

void section_stack_unwind() {
    std::cout << "\n=== Stack Unwinding ===\n";
    try {
        might_throw(true);
    }
    catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
    }
    // Both 'a' and 'b' are destroyed even though exception was thrown
    // 'c' was never created so not destroyed
}

// ── Exception Safety Guarantees ───────────────────────────────────────────────
/*
 * Four levels (weakest to strongest):
 *
 * No-throw guarantee (noexcept):
 *   Function never throws. Destructors, swap(), move ops should be noexcept.
 *
 * Strong guarantee (commit-or-rollback):
 *   If it throws, state is exactly as before the call — like a transaction.
 *
 * Basic guarantee:
 *   If it throws, no resources are leaked and all invariants are maintained.
 *   (State may have changed, but it's valid.)
 *
 * No guarantee:
 *   An exception may leave the program in an invalid state — AVOID.
 */

class TransactionBuffer {
    std::vector<int> committed_;
    std::vector<int> pending_;

public:
    // Strong guarantee — either all committed or nothing changed
    void commit() {
        // Work on a copy, only swap at the end (copy-and-swap idiom)
        std::vector<int> temp = committed_;
        for (auto x : pending_) temp.push_back(x);  // might throw
        committed_.swap(temp);  // noexcept — only done if above succeeded
        pending_.clear();
    }

    void stage(int v) { pending_.push_back(v); }
    size_t size() const { return committed_.size(); }
};

// ── noexcept ─────────────────────────────────────────────────────────────────
// Declares a function won't throw.
// - Enables compiler optimizations (no unwind tables needed)
// - Required for move constructors to be used in certain STL operations
// - If a noexcept function DOES throw, std::terminate() is called

int safe_add(int a, int b) noexcept {
    return a + b;    // arithmetic never throws
}

// noexcept(expr) — conditionally noexcept based on an expression
template<typename T>
void safe_swap(T& a, T& b) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                     std::is_nothrow_move_assignable_v<T>) {
    T temp = std::move(a);
    a      = std::move(b);
    b      = std::move(temp);
}

void section_noexcept() {
    std::cout << "\n=== noexcept ===\n";
    std::cout << "safe_add(3,4) = " << safe_add(3, 4) << "\n";

    // noexcept operator — test at compile time whether expression throws
    std::cout << "noexcept(safe_add(1,2)): " << noexcept(safe_add(1,2)) << "\n";  // 1
    std::cout << "noexcept(std::string()): " << noexcept(std::string())  << "\n";  // 0

    int x = 1, y = 2;
    safe_swap(x, y);
    std::cout << "swapped: x=" << x << " y=" << y << "\n";
}

// ── Standard Exception Hierarchy ──────────────────────────────────────────────
/*
 * std::exception
 * ├── std::bad_alloc          — new fails (out of memory)
 * ├── std::bad_cast           — dynamic_cast fails (reference form)
 * ├── std::bad_typeid         — typeid on null pointer
 * ├── std::bad_exception
 * └── std::logic_error        — programming errors, detectable before runtime
 *     ├── std::invalid_argument
 *     ├── std::domain_error
 *     ├── std::length_error
 *     └── std::out_of_range
 * └── std::runtime_error      — errors during execution
 *     ├── std::overflow_error
 *     ├── std::underflow_error
 *     └── std::range_error
 */
void section_hierarchy() {
    std::cout << "\n=== Standard Exception Hierarchy ===\n";

    // bad_alloc — when new can't allocate
    try {
        // Don't actually allocate huge memory — just demonstrate catching
        throw std::bad_alloc();
    }
    catch (const std::bad_alloc& e) {
        std::cout << "bad_alloc: " << e.what() << "\n";
    }

    // vector::at() throws out_of_range
    try {
        std::vector<int> v = {1,2,3};
        v.at(10);   // out of bounds
    }
    catch (const std::out_of_range& e) {
        std::cout << "out_of_range: " << e.what() << "\n";
    }

    // bad_cast — dynamic_cast on reference fails
    try {
        struct Base { virtual ~Base(){} };
        struct Derived : Base {};
        Base b;
        (void)dynamic_cast<Derived&>(b);   // throws std::bad_cast
    }
    catch (const std::bad_cast& e) {
        std::cout << "bad_cast: " << e.what() << "\n";
    }

    // Catching by base class — catches all derived exceptions
    try {
        throw std::overflow_error("integer overflow");
    }
    catch (const std::exception& e) {
        // catches ANY std::exception derivative
        std::cout << "std::exception: " << e.what() << "\n";
    }
}

// ── RAII and exceptions ────────────────────────────────────────────────────────
// Smart pointers ensure cleanup even if exceptions are thrown mid-function.
void section_raii_exceptions() {
    std::cout << "\n=== RAII + Exceptions ===\n";

    struct Bomb {
        ~Bomb() { std::cout << "  Bomb defused (destructor ran)\n"; }
    };

    try {
        auto ptr = std::make_unique<Bomb>();   // allocated
        // ... some code ...
        throw std::runtime_error("mid-function throw");
        // ptr would go out of scope HERE normally, but exception interrupts
    }
    catch (const std::exception& e) {
        std::cout << "  caught: " << e.what() << "\n";
        // unique_ptr destructor already ran during stack unwind — no leak!
    }
}

int main() {
    std::cout << "╔══ Topic 11: Exception Handling ══╗\n";

    section_basic();
    section_multiple_catch();
    section_custom_exceptions();
    section_propagation();
    section_stack_unwind();
    section_noexcept();
    section_hierarchy();
    section_raii_exceptions();

    std::cout << "\n✓ Topic 11 complete.\n";
}
