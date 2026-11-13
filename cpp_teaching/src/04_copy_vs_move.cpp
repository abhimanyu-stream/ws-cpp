/**
 * LESSON 04 — Copy Constructor vs Move Constructor
 * =================================================
 * Key ideas:
 *  - Copy constructor duplicates an object (expensive for heap resources).
 *  - Move constructor STEALS resources from an rvalue (cheap — no allocation).
 *  - Rule of Five: if you define any of {destructor, copy-ctor, copy-assign,
 *    move-ctor, move-assign}, define all five.
 *  - std::move casts to rvalue; it does NOT move by itself.
 *  - After a move, the source is in a valid-but-unspecified state.
 */

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include <algorithm>

// ── A simple Buffer implementing Rule of Five ────────────────────────────────
class Buffer {
public:
    // Constructor
    explicit Buffer(std::size_t size, char fill = 0)
        : size_(size), data_(new char[size]) {
        std::memset(data_, fill, size_);
        std::cout << "  Buffer(" << size_ << ") constructed\n";
    }

    // 1. Destructor
    ~Buffer() {
        delete[] data_;
        std::cout << "  ~Buffer(" << size_ << ") destroyed\n";
    }

    // 2. Copy constructor — deep copy
    Buffer(const Buffer& other)
        : size_(other.size_), data_(new char[other.size_]) {
        std::memcpy(data_, other.data_, size_);
        std::cout << "  Buffer copy-ctor (size=" << size_ << ")\n";
    }

    // 3. Copy assignment — deep copy
    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;       // self-assign guard
        delete[] data_;
        size_ = other.size_;
        data_ = new char[size_];
        std::memcpy(data_, other.data_, size_);
        std::cout << "  Buffer copy-assign (size=" << size_ << ")\n";
        return *this;
    }

    // 4. Move constructor — steal resources (no allocation)
    Buffer(Buffer&& other) noexcept
        : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;              // leave source in valid state
        std::cout << "  Buffer move-ctor\n";
    }

    // 5. Move assignment — steal resources
    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;
        delete[] data_;
        size_ = other.size_;
        data_ = other.data_;
        other.size_ = 0;
        other.data_ = nullptr;
        std::cout << "  Buffer move-assign\n";
        return *this;
    }

    std::size_t size() const { return size_; }
    char*       data()       { return data_; }

private:
    std::size_t size_;
    char*       data_;
};

// ── 1. Copy vs Move demonstration ───────────────────────────────────────────
void copy_vs_move_demo() {
    std::cout << "\n=== Copy vs Move ===\n";

    Buffer b1(1024, 'A');

    std::cout << "\n--- Copy construction ---\n";
    Buffer b2 = b1;                     // copy ctor called
    std::cout << "b1.size()=" << b1.size() << "  b2.size()=" << b2.size() << "\n";

    std::cout << "\n--- Move construction ---\n";
    Buffer b3 = std::move(b1);          // move ctor called
    std::cout << "b1.size()=" << b1.size() << " (empty)  b3.size()=" << b3.size() << "\n";

    std::cout << "\n--- Scope end (all destroyed) ---\n";
}

// ── 2. std::move with STL types ──────────────────────────────────────────────
void stl_move_demo() {
    std::cout << "\n=== std::move with std::string ===\n";

    std::string s1 = "Hello, World!";
    std::string s2 = s1;                // copy — s1 unchanged
    std::string s3 = std::move(s1);     // move — s1 may be empty

    std::cout << "s1 after move: '" << s1 << "'\n";   // likely empty
    std::cout << "s2 (copy):     '" << s2 << "'\n";
    std::cout << "s3 (moved):    '" << s3 << "'\n";

    // Vector: push_back with move avoids copy
    std::cout << "\n=== push_back copy vs move ===\n";
    std::vector<std::string> v;
    v.reserve(2);

    std::string a = "CopyMe";
    std::string b = "MoveMe";
    v.push_back(a);             // copy — a still valid
    v.push_back(std::move(b));  // move — b now unspecified

    std::cout << "a after push_back(a):        '" << a << "'\n";
    std::cout << "b after push_back(move(b)):  '" << b << "'\n";
}

// ── 3. Return Value Optimisation (RVO / NRVO) ───────────────────────────────
Buffer make_buffer(std::size_t n) {
    // Compiler applies NRVO: the Buffer is constructed directly in the
    // caller's storage — no copy or move occurs at all.
    Buffer result(n, 'Z');
    return result;
}

void rvo_demo() {
    std::cout << "\n=== RVO / NRVO ===\n";
    // Expect exactly ONE constructor call (no copy/move printed):
    Buffer b = make_buffer(512);
    std::cout << "b.size()=" << b.size() << "\n";
}

// ── 4. Perfect forwarding with universal reference ───────────────────────────
template<typename T>
void log_and_store(std::vector<std::string>& vec, T&& val) {
    // Forward preserves value category (lvalue stays lvalue, rvalue stays rvalue)
    vec.emplace_back(std::forward<T>(val));
}

void perfect_forward_demo() {
    std::cout << "\n=== Perfect Forwarding ===\n";
    std::vector<std::string> store;
    std::string lval = "lvalue";
    log_and_store(store, lval);                 // copy (lvalue forwarded)
    log_and_store(store, "rvalue literal");     // move (rvalue forwarded)
    for (auto& s : store) std::cout << "  " << s << "\n";
}

// ── 5. Rule of Zero (prefer this when possible) ──────────────────────────────
struct Point {           // no manual resource management needed
    double x, y;
};                       // compiler-generated Big Five are correct & free

void rule_of_zero_demo() {
    std::cout << "\n=== Rule of Zero ===\n";
    Point p1{1.0, 2.0};
    Point p2 = p1;          // compiler-generated copy is fine
    Point p3 = std::move(p1); // compiler-generated move is fine
    std::cout << "p2=(" << p2.x << "," << p2.y << ")  "
              << "p3=(" << p3.x << "," << p3.y << ")\n";
}

int main() {
    copy_vs_move_demo();
    stl_move_demo();
    rvo_demo();
    perfect_forward_demo();
    rule_of_zero_demo();

    std::cout << "\n✓ Lesson 04 complete.\n";
}
