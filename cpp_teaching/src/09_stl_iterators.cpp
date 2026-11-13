/**
 * LESSON 09 — STL Iterators
 * ==========================
 * Key ideas:
 *  - Iterator categories: input, output, forward, bidirectional, random-access,
 *    contiguous (C++17/20).
 *  - std::advance, std::distance, std::next, std::prev.
 *  - Iterator adapters: reverse_iterator, insert_iterator, stream iterators.
 *  - Writing a simple custom forward iterator.
 *  - Range-based for under the hood.
 */

#include <iostream>
#include <vector>
#include <list>
#include <forward_list>
#include <deque>
#include <set>
#include <string>
#include <string_view>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <cassert>

template<typename C>
void print(std::string_view label, const C& c) {
    std::cout << "  " << label << ": [ ";
    for (const auto& v : c) std::cout << v << " ";
    std::cout << "]\n";
}

// ── 1. Iterator categories ───────────────────────────────────────────────────
void category_demo() {
    std::cout << "\n=== Iterator Categories ===\n";

    // Input iterator — single-pass, read-only  (e.g. std::istream_iterator)
    // Output iterator — single-pass, write-only (e.g. std::ostream_iterator)
    // Forward iterator — multi-pass, forward only (e.g. std::forward_list<>::iterator)
    // Bidirectional — forward + backward (e.g. std::list<>::iterator, std::set<>::iterator)
    // Random-access — O(1) ± offset (e.g. std::vector<>::iterator, std::deque<>::iterator)
    // Contiguous (C++17) — random-access + contiguous memory (std::vector, std::array)

    std::vector<int>       v = {1,2,3,4,5};
    std::list<int>         l = {1,2,3,4,5};
    std::forward_list<int> f = {1,2,3,4,5};

    // Random-access: arithmetic on iterators
    auto vit = v.begin();
    vit += 3;                            // allowed for random-access
    std::cout << "  vector it += 3  → " << *vit << "\n";

    // Bidirectional: decrement allowed, arithmetic not
    auto lit = l.end();
    --lit;                               // ok
    // lit += 2;                         // ERROR: not random-access
    std::cout << "  list  --end()   → " << *lit << "\n";

    // Forward: only increment
    auto fit = f.begin();
    ++fit; ++fit;
    std::cout << "  flist +2        → " << *fit << "\n";
}

// ── 2. Iterator utilities ────────────────────────────────────────────────────
void utilities_demo() {
    std::cout << "\n=== std::advance / distance / next / prev ===\n";

    std::list<int> l = {10, 20, 30, 40, 50};
    auto it = l.begin();

    std::advance(it, 2);                 // O(n) for bidirectional/forward
    std::cout << "  advance(2) → " << *it << "\n";

    auto it2 = std::next(it, 1);        // returns new iterator, doesn't modify
    auto it3 = std::prev(it, 1);
    std::cout << "  next(1) → " << *it2 << "  prev(1) → " << *it3 << "\n";

    auto dist = std::distance(l.begin(), it);  // O(n) for non-random-access
    std::cout << "  distance from begin = " << dist << "\n";

    // For random-access, all operations are O(1):
    std::vector<int> v = {10,20,30,40,50};
    auto vit = v.begin() + 2;           // O(1)
    std::cout << "  vector begin+2 → " << *vit << "\n";
    std::cout << "  vector distance = " << (v.end() - v.begin()) << "\n";
}

// ── 3. Reverse iterators ─────────────────────────────────────────────────────
void reverse_iterator_demo() {
    std::cout << "\n=== reverse_iterator ===\n";

    std::vector<int> v = {1,2,3,4,5};

    // rbegin/rend traverse backwards
    std::cout << "  reverse: [ ";
    for (auto it = v.rbegin(); it != v.rend(); ++it)
        std::cout << *it << " ";
    std::cout << "]\n";

    // Sort descending using reverse iterators
    std::vector<int> u = {5,2,8,1,9};
    std::sort(u.rbegin(), u.rend());
    print("sorted desc via rbegin/rend", u);

    // base() converts a reverse_iterator back to its underlying iterator
    // Note: r.base() points one element *after* what *r dereferences.
    auto rit = v.rbegin();              // points at v[4] = 5
    auto fwd = rit.base();             // points at v.end()
    --fwd;                             // now points at v[4]
    std::cout << "  rbegin() value=" << *rit
              << "  base()-1 value=" << *fwd << "\n";
}

// ── 4. Insert iterators ──────────────────────────────────────────────────────
void insert_iterator_demo() {
    std::cout << "\n=== Insert Iterators ===\n";

    std::vector<int> src = {3,4,5};
    std::vector<int> dst = {1,2,6,7};

    // back_inserter — calls push_back
    std::vector<int> back_result;
    std::copy(src.begin(), src.end(), std::back_inserter(back_result));
    print("back_inserter", back_result);

    // front_inserter — calls push_front (not available for vector)
    std::list<int> front_result = {4,5};
    std::copy(src.begin(), src.end(), std::front_inserter(front_result));
    print("front_inserter into list", front_result);

    // inserter — insert at a specific position
    auto pos = std::find(dst.begin(), dst.end(), 6);
    std::copy(src.begin(), src.end(), std::inserter(dst, pos));
    print("inserter before 6", dst);
}

// ── 5. Stream iterators ──────────────────────────────────────────────────────
void stream_iterator_demo() {
    std::cout << "\n=== Stream Iterators ===\n";

    // ostream_iterator — write to a stream as if it were a container
    std::vector<int> v = {1,2,3,4,5};
    std::cout << "  ostream_iterator: ";
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "\n";

    // istream_iterator — read from a stream as if it were a range
    std::istringstream iss("10 20 30 40 50");
    std::vector<int> parsed{std::istream_iterator<int>(iss),
                            std::istream_iterator<int>()};  // EOF sentinel
    print("istream_iterator parsed", parsed);

    // Pipeline: read from string, square, write to cout
    std::istringstream iss2("1 2 3 4 5");
    std::transform(std::istream_iterator<int>(iss2),
                   std::istream_iterator<int>(),
                   std::ostream_iterator<int>(std::cout, " "),
                   [](int x){ return x * x; });
    std::cout << "  ← squared via stream iterators\n";
}

// ── 6. Custom forward iterator ───────────────────────────────────────────────
// A simple stride iterator that visits every Nth element of a vector.
template<typename T>
class stride_iterator {
public:
    // Five iterator traits required pre-C++20
    using iterator_category = std::forward_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    stride_iterator(T* ptr, std::size_t stride)
        : ptr_(ptr), stride_(stride) {}

    reference operator*()  const { return *ptr_; }
    pointer   operator->() const { return ptr_;  }

    stride_iterator& operator++()    { ptr_ += stride_; return *this; }   // pre-increment
    stride_iterator  operator++(int) {                                     // post-increment
        auto tmp = *this; ++(*this); return tmp;
    }

    bool operator==(const stride_iterator& o) const { return ptr_ == o.ptr_; }
    bool operator!=(const stride_iterator& o) const { return ptr_ != o.ptr_; }

private:
    T*          ptr_;
    std::size_t stride_;
};

void custom_iterator_demo() {
    std::cout << "\n=== Custom Stride Iterator ===\n";

    std::vector<int> v = {0,1,2,3,4,5,6,7,8,9};

    // Visit every 3rd element starting at index 0
    std::size_t stride = 3;
    // End pointer: advance past the last valid stride-aligned element
    std::size_t count = (v.size() + stride - 1) / stride;
    auto begin = stride_iterator<int>(v.data(), stride);
    auto end   = stride_iterator<int>(v.data() + count * stride, stride);

    std::cout << "  every 3rd element: [ ";
    for (auto it = begin; it != end; ++it)
        std::cout << *it << " ";
    std::cout << "]\n";

    // Works with standard algorithms
    int sum = std::accumulate(begin, end, 0);
    std::cout << "  sum of strided elements = " << sum << "\n";
}

// ── 7. Range-based for under the hood ───────────────────────────────────────
void range_for_demo() {
    std::cout << "\n=== Range-based for — Under the Hood ===\n";

    std::vector<int> v = {10, 20, 30};

    // This range-based for:
    //   for (auto& x : v) ...
    //
    // Desugars to:
    //   auto&& __range = v;
    //   auto   __begin = __range.begin();
    //   auto   __end   = __range.end();
    //   for (; __begin != __end; ++__begin) {
    //       auto& x = *__begin;
    //       ...
    //   }

    std::cout << "  range-based for  : [ ";
    for (auto& x : v) std::cout << x << " ";
    std::cout << "]\n";

    std::cout << "  manual equivalent: [ ";
    {
        auto&& __range = v;
        auto __begin = __range.begin();
        auto __end   = __range.end();
        for (; __begin != __end; ++__begin) {
            auto& x = *__begin;
            std::cout << x << " ";
        }
    }
    std::cout << "]\n";

    // Any type with begin()/end() (or free begin/end) works:
    int raw[] = {1,2,3,4,5};
    std::cout << "  raw array       : [ ";
    for (auto x : raw) std::cout << x << " ";
    std::cout << "]\n";
}

// ── 8. std::span (C++20 preview) ─────────────────────────────────────────────
// span is a lightweight, non-owning view over contiguous data — essentially
// a pointer + size. It has begin/end so it works with all STL algorithms.
//
// Requires C++20 (#include <span>). Shown here as a concept summary since
// this project targets C++17; uncomment if you switch to C++20.
//
// void span_demo() {
//     #include <span>
//     std::vector<int> v = {1,2,3,4,5};
//     std::span<int> s(v);                // view over entire vector
//     std::span<int> first3 = s.first(3); // view over first 3 elements
//     for (int x : first3) std::cout << x << " ";
// }

int main() {
    category_demo();
    utilities_demo();
    reverse_iterator_demo();
    insert_iterator_demo();
    stream_iterator_demo();
    custom_iterator_demo();
    range_for_demo();

    std::cout << "\n✓ Lesson 09 complete.\n";
}
