/**
 * LESSON 08 — STL Algorithms
 * ===========================
 * Covers: sorting, searching, transforming, partitioning, numeric algorithms,
 *         C++17 parallel execution policies, and std::optional / std::variant.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <execution>   // C++17 parallel policies
#include <random>
#include <iterator>
#include <sstream>

template<typename C>
void print(std::string_view label, const C& c) {
    std::cout << "  " << label << ": [ ";
    for (auto& v : c) std::cout << v << " ";
    std::cout << "]\n";
}

// ── 1. Sorting family ────────────────────────────────────────────────────────
void sorting_demo() {
    std::cout << "\n=== Sorting ===\n";

    std::vector<int> v = {5,3,8,1,9,2,7,4,6};

    std::sort(v.begin(), v.end());                          // unstable O(n log n)
    print("sort ascending", v);

    std::sort(v.begin(), v.end(), std::greater<int>{});     // descending
    print("sort descending", v);

    // stable_sort preserves equal element order
    struct Item { int priority; std::string name; };
    std::vector<Item> items = {{2,"B"},{1,"C"},{2,"A"},{1,"D"}};
    std::stable_sort(items.begin(), items.end(),
        [](const Item& a, const Item& b){ return a.priority < b.priority; });
    std::cout << "  stable_sort by priority: ";
    for (auto& it : items) std::cout << it.name << "(" << it.priority << ") ";
    std::cout << "\n";

    // partial_sort — only sort first k elements
    std::vector<int> u = {5,3,8,1,9,2,7};
    std::partial_sort(u.begin(), u.begin()+3, u.end());
    print("partial_sort(3)", u);

    // nth_element — O(n) pivot: element at position n is correct
    std::vector<int> w = {5,3,8,1,9,2,7};
    std::nth_element(w.begin(), w.begin()+3, w.end());
    std::cout << "  nth_element(3): w[3]=" << w[3] << " (4th smallest)\n";
}

// ── 2. Searching family ──────────────────────────────────────────────────────
void searching_demo() {
    std::cout << "\n=== Searching ===\n";

    std::vector<int> v = {1,2,3,4,5,6,7,8,9};

    // binary_search (requires sorted range)
    std::cout << "  binary_search(5): " << std::boolalpha
              << std::binary_search(v.begin(), v.end(), 5) << "\n";

    // lower_bound / upper_bound
    auto lb = std::lower_bound(v.begin(), v.end(), 4);  // first ≥ 4
    auto ub = std::upper_bound(v.begin(), v.end(), 6);  // first > 6
    std::cout << "  lower_bound(4)=" << *lb << "  upper_bound(6)=" << *ub << "\n";

    // equal_range
    std::vector<int> m = {1,2,2,2,3,4};
    auto [lo, hi] = std::equal_range(m.begin(), m.end(), 2);
    std::cout << "  equal_range(2): count=" << std::distance(lo, hi) << "\n";

    // find / find_if
    auto it = std::find_if(v.begin(), v.end(), [](int x){ return x > 6; });
    std::cout << "  first element > 6: " << *it << "\n";

    // min / max / minmax element
    auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    std::cout << "  min=" << *mn << "  max=" << *mx << "\n";
}

// ── 3. Transform / for_each / generate ──────────────────────────────────────
void transform_demo() {
    std::cout << "\n=== Transform ===\n";

    std::vector<int> v = {1,2,3,4,5};
    std::vector<int> out(v.size());

    std::transform(v.begin(), v.end(), out.begin(), [](int x){ return x * x; });
    print("squares", out);

    // in-place
    std::for_each(v.begin(), v.end(), [](int& x){ x *= 2; });
    print("doubled in-place", v);

    // generate
    std::vector<int> rnd(8);
    std::mt19937 rng(42);
    std::generate(rnd.begin(), rnd.end(), [&]{ return rng() % 100; });
    print("random", rnd);

    // iota — fill with consecutive values
    std::vector<int> seq(10);
    std::iota(seq.begin(), seq.end(), 1);
    print("iota 1..10", seq);
}

// ── 4. Copy / fill / replace ─────────────────────────────────────────────────
void copy_fill_demo() {
    std::cout << "\n=== Copy / Fill / Replace ===\n";

    std::vector<int> src = {1,2,3,4,5};
    std::vector<int> dst;
    std::copy(src.begin(), src.end(), std::back_inserter(dst));
    print("copy", dst);

    std::fill(dst.begin(), dst.begin()+3, 0);
    print("fill first 3 with 0", dst);

    std::replace(dst.begin(), dst.end(), 0, 99);
    print("replace 0→99", dst);

    std::vector<int> v2;
    std::copy_if(src.begin(), src.end(), std::back_inserter(v2),
                 [](int x){ return x % 2 == 0; });
    print("copy_if even", v2);
}

// ── 5. Numeric algorithms ────────────────────────────────────────────────────
void numeric_demo() {
    std::cout << "\n=== Numeric Algorithms ===\n";

    std::vector<int> v = {1,2,3,4,5};

    int sum  = std::accumulate(v.begin(), v.end(), 0);
    int prod = std::accumulate(v.begin(), v.end(), 1, std::multiplies<int>{});
    std::cout << "  sum=" << sum << "  product=" << prod << "\n";

    // C++17: std::reduce (can be parallelized)
    int rsum = std::reduce(v.begin(), v.end(), 0);
    std::cout << "  reduce sum=" << rsum << "\n";

    // partial_sum (prefix sums / running total)
    std::vector<int> prefix(v.size());
    std::partial_sum(v.begin(), v.end(), prefix.begin());
    print("partial_sum", prefix);

    // adjacent_difference
    std::vector<int> diff(v.size());
    std::adjacent_difference(v.begin(), v.end(), diff.begin());
    print("adjacent_diff", diff);

    // inner_product (dot product)
    std::vector<int> a = {1,2,3}, b = {4,5,6};
    int dot = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    std::cout << "  dot product=" << dot << "\n";
}

// ── 6. Partitioning ──────────────────────────────────────────────────────────
void partition_demo() {
    std::cout << "\n=== Partitioning ===\n";

    std::vector<int> v = {1,2,3,4,5,6,7,8,9};
    auto mid = std::partition(v.begin(), v.end(), [](int x){ return x % 2 == 0; });
    print("partitioned (evens first)", v);
    std::cout << "  partition point index=" << std::distance(v.begin(), mid) << "\n";

    std::cout << "  is_partitioned: "
              << std::is_partitioned(v.begin(), v.end(), [](int x){ return x%2==0; }) << "\n";
}

// ── 7. Permutations ──────────────────────────────────────────────────────────
void permutation_demo() {
    std::cout << "\n=== Permutations ===\n";
    std::vector<int> v = {1,2,3};
    do {
        for (int x : v) std::cout << x;
        std::cout << " ";
    } while (std::next_permutation(v.begin(), v.end()));
    std::cout << "\n";
}

// ── 8. C++17 parallel execution ──────────────────────────────────────────────
void parallel_demo() {
    std::cout << "\n=== C++17 Parallel Execution (conceptual) ===\n";
    // Requires linking against TBB on many platforms.
    // Uncommenting below enables parallel sort:
    //
    // std::vector<int> big(1'000'000);
    // std::iota(big.begin(), big.end(), 0);
    // std::shuffle(big.begin(), big.end(), std::mt19937{42});
    // std::sort(std::execution::par_unseq, big.begin(), big.end());
    //
    // Policies:
    //   std::execution::seq        — sequential (default)
    //   std::execution::par        — parallel
    //   std::execution::par_unseq  — parallel + vectorised (SIMD)
    std::cout << "  (enable by uncommenting the parallel sort block above)\n";
}

// ── 9. std::optional (C++17) ─────────────────────────────────────────────────
std::optional<int> safe_divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

void optional_demo() {
    std::cout << "\n=== std::optional ===\n";

    if (auto r = safe_divide(10, 3)) std::cout << "  10/3=" << *r << "\n";
    if (auto r = safe_divide(10, 0)) std::cout << "  10/0=" << *r << "\n";
    else std::cout << "  10/0 → no value (division by zero)\n";

    // value_or — provide default
    std::cout << "  value_or(99)=" << safe_divide(10, 0).value_or(99) << "\n";
}

// ── 10. std::variant (C++17) ─────────────────────────────────────────────────
void variant_demo() {
    std::cout << "\n=== std::variant ===\n";

    using Val = std::variant<int, double, std::string>;
    std::vector<Val> vals = {42, 3.14, std::string("hello"), 7};

    for (const auto& v : vals) {
        std::visit([](const auto& x){
            std::cout << "  " << x << " (type: "
                      << typeid(x).name() << ")\n";
        }, v);
    }

    // Pattern matching via overloaded visitor
    struct Visitor {
        void operator()(int i)               const { std::cout << "  int: "    << i << "\n"; }
        void operator()(double d)            const { std::cout << "  double: " << d << "\n"; }
        void operator()(const std::string& s)const { std::cout << "  string: " << s << "\n"; }
    };
    std::visit(Visitor{}, vals[2]);
}

int main() {
    sorting_demo();
    searching_demo();
    transform_demo();
    copy_fill_demo();
    numeric_demo();
    partition_demo();
    permutation_demo();
    parallel_demo();
    optional_demo();
    variant_demo();

    std::cout << "\n✓ Lesson 08 complete.\n";
}
