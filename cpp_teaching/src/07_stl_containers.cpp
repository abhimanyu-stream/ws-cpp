/**
 * LESSON 07 — STL Containers: vector, list, deque, map, unordered_map, set,
 *             multimap, multiset, stack, queue, priority_queue, array, span
 * ==========================================================================
 * Key ideas:
 *  - Container choice is O(n) vs O(log n) vs O(1) amortized.
 *  - Iterators provide uniform traversal interface.
 *  - C++17 additions: std::string_view, structured bindings, if-init,
 *    std::optional, node handles.
 */

#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <multiset>       // ← via <set>
#include <unordered_set>
#include <stack>
#include <queue>
#include <string>
#include <string_view>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cassert>

// ── Helper ───────────────────────────────────────────────────────────────────
template<typename C>
void print_range(std::string_view label, const C& c) {
    std::cout << "  " << label << ": [ ";
    for (const auto& v : c) std::cout << v << " ";
    std::cout << "]\n";
}

// ── 1. std::vector ───────────────────────────────────────────────────────────
void vector_demo() {
    std::cout << "\n=== std::vector (dynamic array) ===\n";

    std::vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};
    v.reserve(20);                     // avoid re-alloc
    v.push_back(7);
    v.emplace_back(8);                 // in-place construction (faster for objects)

    print_range("v", v);
    std::cout << "  size=" << v.size() << "  capacity=" << v.capacity() << "\n";

    // Random access O(1)
    std::cout << "  v[3]=" << v[3] << "  v.at(3)=" << v.at(3) << "\n";

    // Erase-remove idiom (C++17)
    v.erase(std::remove(v.begin(), v.end(), 1), v.end());
    print_range("after removing 1s", v);

    // Sorted and binary search
    std::sort(v.begin(), v.end());
    bool found = std::binary_search(v.begin(), v.end(), 5);
    std::cout << "  sorted, 5 found=" << std::boolalpha << found << "\n";

    // 2D vector
    std::vector<std::vector<int>> matrix(3, std::vector<int>(3, 0));
    matrix[1][1] = 9;
    std::cout << "  matrix[1][1]=" << matrix[1][1] << "\n";
}

// ── 2. std::array ────────────────────────────────────────────────────────────
void array_demo() {
    std::cout << "\n=== std::array (fixed-size on stack) ===\n";

    std::array<int, 5> a = {5, 4, 3, 2, 1};
    std::sort(a.begin(), a.end());
    print_range("sorted array", a);
    std::cout << "  size (compile-time)=" << a.size() << "\n";
}

// ── 3. std::list ─────────────────────────────────────────────────────────────
void list_demo() {
    std::cout << "\n=== std::list (doubly-linked) ===\n";

    std::list<int> lst = {1, 2, 3, 4, 5};
    lst.push_front(0);
    lst.push_back(6);
    lst.insert(std::next(lst.begin(), 3), 99);  // O(1) insert after iterator

    print_range("list", lst);
    lst.remove(99);
    lst.sort();
    lst.unique();   // remove consecutive duplicates
    print_range("after sort+unique", lst);
}

// ── 4. std::deque ────────────────────────────────────────────────────────────
void deque_demo() {
    std::cout << "\n=== std::deque (double-ended queue) ===\n";

    std::deque<int> dq;
    dq.push_front(2);
    dq.push_front(1);
    dq.push_back(3);
    dq.push_back(4);
    print_range("deque", dq);
    dq.pop_front();
    print_range("after pop_front", dq);
}

// ── 5. std::map ──────────────────────────────────────────────────────────────
void map_demo() {
    std::cout << "\n=== std::map (sorted, O(log n)) ===\n";

    std::map<std::string, int> scores;
    scores["Alice"] = 95;
    scores["Bob"]   = 87;
    scores.emplace("Charlie", 91);
    scores["Alice"] += 3;   // modify existing

    // C++17 structured bindings
    for (const auto& [name, score] : scores)
        std::cout << "  " << name << " → " << score << "\n";

    // C++17 if-init
    if (auto it = scores.find("Bob"); it != scores.end())
        std::cout << "  Found Bob: " << it->second << "\n";

    // insert_or_assign (C++17)
    auto [iter, inserted] = scores.insert_or_assign("Dave", 78);
    std::cout << "  Dave inserted=" << inserted << "  score=" << iter->second << "\n";

    // Node handle splicing (C++17)
    std::map<std::string, int> other;
    auto node = scores.extract("Bob");     // remove without destroying
    node.key() = "Robert";
    other.insert(std::move(node));         // insert into other map
    std::cout << "  other[Robert]=" << other["Robert"] << "\n";
}

// ── 6. std::unordered_map ────────────────────────────────────────────────────
void unordered_map_demo() {
    std::cout << "\n=== std::unordered_map (hash table, avg O(1)) ===\n";

    std::unordered_map<std::string, int> freq;
    std::string text = "the quick brown fox jumps over the lazy fox";
    for (char c : text) if (c != ' ') freq[std::string(1,c)]++;

    // try_emplace (C++17) — only inserts if key absent, no copy if key present
    freq.try_emplace("z", 0);

    // reserve buckets for expected elements (reduces rehashing)
    std::unordered_map<int, std::string> m;
    m.reserve(100);
    m.max_load_factor(0.7f);

    for (int i = 0; i < 5; ++i) m[i] = "val" + std::to_string(i);
    std::cout << "  m[2]=" << m[2] << "  bucket_count=" << m.bucket_count() << "\n";
}

// ── 7. std::set / std::unordered_set ────────────────────────────────────────
void set_demo() {
    std::cout << "\n=== std::set (sorted unique) ===\n";

    std::set<int> s = {5, 2, 8, 1, 9, 2, 5};  // duplicates dropped
    print_range("set", s);

    s.insert(7);
    auto [it, ok] = s.insert(7);    // duplicate — ok=false
    std::cout << "  re-insert 7: ok=" << ok << "\n";

    // Set operations
    std::set<int> a = {1,2,3,4}, b = {3,4,5,6};
    std::vector<int> inter, uni;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(inter));
    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(uni));
    print_range("intersection", inter);
    print_range("union", uni);

    std::cout << "\n=== std::unordered_set (hash, avg O(1)) ===\n";
    std::unordered_set<std::string> words = {"apple","banana","cherry","apple"};
    std::cout << "  size=" << words.size() << " (duplicate 'apple' removed)\n";
    std::cout << "  contains 'banana': " << words.count("banana") << "\n";
}

// ── 8. std::multimap / std::multiset ────────────────────────────────────────
void multi_demo() {
    std::cout << "\n=== multimap / multiset ===\n";

    std::multimap<int, std::string> mm;
    mm.insert({1, "one-a"});
    mm.insert({1, "one-b"});
    mm.insert({2, "two"});

    auto range = mm.equal_range(1);
    std::cout << "  entries with key=1:\n";
    for (auto it = range.first; it != range.second; ++it)
        std::cout << "    " << it->first << " → " << it->second << "\n";

    std::multiset<int> ms = {1,1,2,2,2,3};
    std::cout << "  multiset count(2)=" << ms.count(2) << "\n";
}

// ── 9. Stack, Queue, Priority Queue ─────────────────────────────────────────
void adapters_demo() {
    std::cout << "\n=== Container Adaptors ===\n";

    // stack (LIFO)
    std::stack<int> stk;
    for (int i : {1,2,3,4,5}) stk.push(i);
    std::cout << "  stack top=" << stk.top() << "\n";
    stk.pop();
    std::cout << "  after pop top=" << stk.top() << "\n";

    // queue (FIFO)
    std::queue<std::string> q;
    q.push("first"); q.push("second"); q.push("third");
    std::cout << "  queue front='" << q.front() << "'\n";
    q.pop();
    std::cout << "  after pop front='" << q.front() << "'\n";

    // priority_queue (max-heap by default)
    std::priority_queue<int> pq;
    for (int v : {3,1,4,1,5,9,2,6}) pq.push(v);
    std::cout << "  priority_queue top=" << pq.top() << " (max=9)\n";

    // min-heap
    std::priority_queue<int, std::vector<int>, std::greater<int>> minpq;
    for (int v : {3,1,4,1,5}) minpq.push(v);
    std::cout << "  min-heap top=" << minpq.top() << " (min=1)\n";
}

// ── 10. Complexity Summary ───────────────────────────────────────────────────
void complexity_summary() {
    std::cout << R"(
=== Container Complexity Summary ===
  Container         Access   Search   Insert   Delete
  ─────────────────────────────────────────────────────
  vector            O(1)     O(n)     O(1)*    O(n)
  deque             O(1)     O(n)     O(1)**   O(n)
  list              O(n)     O(n)     O(1)†    O(1)†
  array             O(1)     O(n)     —        —
  map               O(log n) O(log n) O(log n) O(log n)
  unordered_map     O(1)     O(1)     O(1)     O(1)
  set               O(log n) O(log n) O(log n) O(log n)
  unordered_set     —        O(1)     O(1)     O(1)
  priority_queue    O(1)top  —        O(log n) O(log n)
  *amortised  **at front/back  †given iterator
)";
}

int main() {
    vector_demo();
    array_demo();
    list_demo();
    deque_demo();
    map_demo();
    unordered_map_demo();
    set_demo();
    multi_demo();
    adapters_demo();
    complexity_summary();

    std::cout << "\n✓ Lesson 07 complete.\n";
}
