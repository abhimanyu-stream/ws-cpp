/**
 * TOPIC 09 — STL — Standard Template Library
 * ============================================
 * Covers: all containers, iterators, algorithms, utilities
 * (pair, tuple, optional, variant, any, function, bind)
 *
 * Compile: g++ -std=c++17 -Wall 09_stl.cpp -o stl
 */

#include <iostream>
#include <string>
// Containers
#include <vector>
#include <list>
#include <deque>
#include <array>
#include <forward_list>
#include <set>          // set + multiset
#include <map>          // map + multimap
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
// Algorithms
#include <algorithm>
#include <numeric>
// Iterators
#include <iterator>
// Utilities
#include <utility>
#include <tuple>
#include <optional>
#include <variant>
#include <any>
#include <functional>

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION A — Containers
// ═══════════════════════════════════════════════════════════════════════════════

// ── vector — dynamic array, O(1) random access, amortized O(1) push_back ─────
void section_vector() {
    std::cout << "\n=== vector ===\n";

    std::vector<int> v = {5, 3, 1, 4, 2};

    v.push_back(6);                   // append
    v.insert(v.begin(), 0);           // insert at front — O(n)
    v.erase(v.begin() + 2);           // erase by iterator — O(n)
    v.emplace_back(7);                // construct in-place (avoid copy)

    std::cout << "size: " << v.size() << " capacity: " << v.capacity() << "\n";
    v.reserve(20);                    // pre-allocate to avoid reallocations
    std::cout << "after reserve: capacity=" << v.capacity() << "\n";
    v.shrink_to_fit();                // release excess capacity

    for (const auto& x : v) std::cout << x << " ";
    std::cout << "\n";

    // 2D vector
    std::vector<std::vector<int>> matrix(3, std::vector<int>(4, 0));
    matrix[1][2] = 42;
    std::cout << "matrix[1][2] = " << matrix[1][2] << "\n";
}

// ── list — doubly-linked list, O(1) insert/erase anywhere ───────────────────
void section_list() {
    std::cout << "\n=== list ===\n";

    std::list<int> l = {3, 1, 4, 1, 5, 9};
    l.push_front(0);           // O(1)
    l.push_back(2);            // O(1)
    l.sort();                  // O(n log n)
    l.unique();                // remove consecutive duplicates
    l.reverse();

    for (auto x : l) std::cout << x << " ";
    std::cout << "\n";

    // splice — move elements between lists in O(1)
    std::list<int> l2 = {100, 200};
    l.splice(l.begin(), l2);   // move all of l2 to front of l
}

// ── deque — double-ended queue, O(1) push/pop both ends ─────────────────────
void section_deque() {
    std::cout << "\n=== deque ===\n";

    std::deque<int> d;
    d.push_back(3);
    d.push_back(4);
    d.push_front(2);
    d.push_front(1);   // 1 2 3 4

    std::cout << "front=" << d.front() << " back=" << d.back() << "\n";
    d.pop_front();
    for (auto x : d) std::cout << x << " ";
    std::cout << "\n";
}

// ── set — sorted unique elements, O(log n) operations ───────────────────────
void section_set() {
    std::cout << "\n=== set / multiset ===\n";

    std::set<int> s = {5, 3, 8, 1, 3, 5};  // duplicates dropped
    s.insert(7);
    s.erase(3);

    for (auto x : s) std::cout << x << " ";  // sorted: 1 5 7 8
    std::cout << "\n";

    std::cout << "count(5): " << s.count(5) << "\n";
    std::cout << "find(7)!=end: " << (s.find(7) != s.end()) << "\n";

    // multiset — allows duplicates
    std::multiset<int> ms = {3, 3, 1, 4, 1, 5};
    std::cout << "multiset count(3): " << ms.count(3) << "\n";
}

// ── map — sorted key-value pairs, O(log n) ───────────────────────────────────
void section_map() {
    std::cout << "\n=== map / multimap ===\n";

    std::map<std::string, int> scores;
    scores["Alice"] = 95;
    scores["Bob"]   = 87;
    scores.insert({"Carol", 92});
    scores.emplace("Dave", 78);

    // Iteration — always sorted by key
    for (const auto& [name, score] : scores)  // structured binding C++17
        std::cout << name << ":" << score << " ";
    std::cout << "\n";

    // Safe lookup
    if (auto it = scores.find("Eve"); it != scores.end())
        std::cout << "Eve: " << it->second << "\n";
    else
        std::cout << "Eve not found\n";

    // operator[] inserts default if key missing — be careful
    scores["Eve"];  // inserts Eve:0

    // at() — throws std::out_of_range if key missing
    try { std::cout << scores.at("Alice") << "\n"; }
    catch (const std::out_of_range& e) { std::cerr << e.what() << "\n"; }
}

// ── unordered_map — hash table, O(1) average ─────────────────────────────────
void section_unordered() {
    std::cout << "\n=== unordered_map / unordered_set ===\n";

    std::unordered_map<std::string, int> freq;
    for (const std::string& word : {"the","quick","brown","fox","the","fox"})
        ++freq[word];

    for (const auto& [word, count] : freq)
        std::cout << word << ":" << count << " ";
    std::cout << "\n";

    std::unordered_set<int> us = {3, 1, 4, 1, 5, 9};  // unique, unordered
    std::cout << "unordered_set size: " << us.size() << "\n";
}

// ── Adapters: stack, queue, priority_queue ────────────────────────────────────
void section_adapters() {
    std::cout << "\n=== stack / queue / priority_queue ===\n";

    // stack — LIFO (last in, first out)
    std::stack<int> st;
    for (int i : {1,2,3,4,5}) st.push(i);
    std::cout << "stack (LIFO): ";
    while (!st.empty()) { std::cout << st.top() << " "; st.pop(); }
    std::cout << "\n";

    // queue — FIFO (first in, first out)
    std::queue<int> q;
    for (int i : {1,2,3,4,5}) q.push(i);
    std::cout << "queue (FIFO): ";
    while (!q.empty()) { std::cout << q.front() << " "; q.pop(); }
    std::cout << "\n";

    // priority_queue — max-heap by default
    std::priority_queue<int> pq;
    for (int i : {5,1,4,2,3}) pq.push(i);
    std::cout << "priority_queue: ";
    while (!pq.empty()) { std::cout << pq.top() << " "; pq.pop(); }
    std::cout << "\n";

    // Min-heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
    for (int i : {5,1,4,2,3}) min_pq.push(i);
    std::cout << "min priority_queue: ";
    while (!min_pq.empty()) { std::cout << min_pq.top() << " "; min_pq.pop(); }
    std::cout << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION B — Iterators
// ═══════════════════════════════════════════════════════════════════════════════
void section_iterators() {
    std::cout << "\n=== Iterators ===\n";

    std::vector<int> v = {1,2,3,4,5};

    // Input / output iterators
    for (auto it = v.begin(); it != v.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    // Reverse iterator
    for (auto it = v.rbegin(); it != v.rend(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    // Const iterator — read-only access
    for (auto it = v.cbegin(); it != v.cend(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    // Iterator arithmetic (random access iterators only — vector, deque, array)
    auto it = v.begin();
    it += 2;    // advance by 2
    std::cout << "it+2: " << *it << "\n";   // 3
    std::cout << "distance: " << std::distance(v.begin(), it) << "\n";

    // std::next / std::prev — advance iterator safely
    auto mid  = std::next(v.begin(), 2);    // points to 3
    auto last = std::prev(v.end(), 1);      // points to 5
    std::cout << "next: " << *mid << " prev: " << *last << "\n";

    // std::advance — in-place advance
    auto it2 = v.begin();
    std::advance(it2, 3);
    std::cout << "after advance(3): " << *it2 << "\n";   // 4

    // Iterator invalidation
    // After vector::push_back (if reallocation) — ALL iterators invalid
    // After vector::insert/erase — iterators at/after insertion point invalid
    // List insert/erase — only erased iterator invalid
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION C — Algorithms
// ═══════════════════════════════════════════════════════════════════════════════
void section_algorithms() {
    std::cout << "\n=== STL Algorithms ===\n";

    std::vector<int> v = {5,3,1,4,2,3,5,1};

    // Sorting
    std::sort(v.begin(), v.end());                            // ascending
    for (auto x : v) std::cout << x << " ";
    std::cout << "\n";

    std::sort(v.begin(), v.end(), std::greater<int>());       // descending
    std::stable_sort(v.begin(), v.end());                     // preserves equal order

    // Searching
    auto it = std::find(v.begin(), v.end(), 3);
    std::cout << "find(3): " << (it != v.end() ? "found" : "not found") << "\n";

    auto it2 = std::find_if(v.begin(), v.end(), [](int x){ return x > 3; });
    std::cout << "find_if(>3): " << *it2 << "\n";

    std::sort(v.begin(), v.end());   // must sort before binary_search
    std::cout << "binary_search(3): " << std::binary_search(v.begin(), v.end(), 3) << "\n";

    auto lb = std::lower_bound(v.begin(), v.end(), 3);  // first pos >= 3
    auto ub = std::upper_bound(v.begin(), v.end(), 3);  // first pos > 3
    std::cout << "lower_bound(3): " << *lb << " upper_bound(3): " << *ub << "\n";

    // Counting
    std::cout << "count(3): "    << std::count(v.begin(), v.end(), 3)                    << "\n";
    std::cout << "count_if(>3): "<< std::count_if(v.begin(), v.end(), [](int x){return x>3;}) << "\n";

    // Transforming
    std::vector<int> doubled(v.size());
    std::transform(v.begin(), v.end(), doubled.begin(), [](int x){ return x*2; });
    for (auto x : doubled) std::cout << x << " ";
    std::cout << "\n";

    // Accumulate (sum)
    int total = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "sum: " << total << "\n";

    int product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>());
    std::cout << "product: " << product << "\n";

    // Reverse, rotate, unique
    std::reverse(v.begin(), v.end());
    std::rotate(v.begin(), v.begin()+2, v.end());  // rotate left by 2
    auto new_end = std::unique(v.begin(), v.end());  // remove consecutive dupes
    v.erase(new_end, v.end());
    for (auto x : v) std::cout << x << " ";
    std::cout << "\n";

    // Partition
    std::vector<int> p = {1,2,3,4,5,6,7,8};
    auto part_it = std::partition(p.begin(), p.end(), [](int x){ return x%2==0; });
    std::cout << "even | odd: ";
    for (auto x : p) std::cout << x << " ";
    std::cout << "| (pivot at index " << std::distance(p.begin(), part_it) << ")\n";

    // min/max
    auto [mn, mx] = std::minmax_element(p.begin(), p.end());
    std::cout << "min=" << *mn << " max=" << *mx << "\n";

    // for_each
    std::cout << "for_each: ";
    std::for_each(p.begin(), p.begin()+4, [](int x){ std::cout << x*x << " "; });
    std::cout << "\n";

    // merge
    std::vector<int> a = {1,3,5}, b2 = {2,4,6};
    std::vector<int> merged(6);
    std::merge(a.begin(), a.end(), b2.begin(), b2.end(), merged.begin());
    for (auto x : merged) std::cout << x << " ";
    std::cout << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// SECTION D — STL Utilities
// ═══════════════════════════════════════════════════════════════════════════════
void section_utilities() {
    std::cout << "\n=== STL Utilities ===\n";

    // pair
    std::pair<int, std::string> p(1, "one");
    auto [num, word] = p;   // structured binding
    std::cout << "pair: " << num << " " << word << "\n";
    auto p2 = std::make_pair(2, "two");
    std::cout << "make_pair: " << p2.first << " " << p2.second << "\n";

    // tuple
    auto t = std::make_tuple(42, 3.14, std::string("hi"));
    auto [i, d, s] = t;    // structured binding
    std::cout << "tuple: " << i << " " << d << " " << s << "\n";
    std::get<0>(t) = 100;  // modify by index

    // optional
    auto safe_div = [](int a, int b) -> std::optional<double> {
        if (b == 0) return std::nullopt;
        return static_cast<double>(a) / b;
    };
    if (auto r = safe_div(10, 3)) std::cout << "div: " << *r << "\n";
    if (!safe_div(10, 0))         std::cout << "div by zero\n";

    // variant
    std::variant<int, double, std::string> v;
    v = 42;
    std::cout << "variant int: " << std::get<int>(v) << "\n";
    v = std::string("hello");
    std::visit([](auto&& val){ std::cout << "visit: " << val << "\n"; }, v);

    // any — holds any type
    std::any a_val = 42;
    std::cout << "any int: " << std::any_cast<int>(a_val) << "\n";
    a_val = std::string("now a string");
    std::cout << "any string: " << std::any_cast<std::string>(a_val) << "\n";

    // std::function
    std::function<int(int,int)> fn = [](int a, int b){ return a+b; };
    std::cout << "std::function: " << fn(3,4) << "\n";

    // std::bind
    using namespace std::placeholders;
    auto add10 = std::bind([](int a, int b){ return a+b; }, 10, _1);
    std::cout << "bind add10(5): " << add10(5) << "\n";

    // reference_wrapper
    int x = 42;
    std::reference_wrapper<int> rw = std::ref(x);
    rw.get() = 99;
    std::cout << "ref_wrapper: x=" << x << "\n";
}

int main() {
    std::cout << "╔══ Topic 09: STL ══╗\n";

    section_vector();
    section_list();
    section_deque();
    section_set();
    section_map();
    section_unordered();
    section_adapters();
    section_iterators();
    section_algorithms();
    section_utilities();

    std::cout << "\n✓ Topic 09 complete.\n";
}
