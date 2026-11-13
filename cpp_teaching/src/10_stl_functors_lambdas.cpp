/**
 * LESSON 10 — Functors & Lambdas
 * ================================
 * Key ideas:
 *  - Function objects (functors): operator() overload.
 *  - std::function — type-erased callable wrapper.
 *  - Lambdas: syntax, capture modes, mutable, generic (C++14/20).
 *  - std::bind and its modern replacements.
 *  - Callable concepts: function pointers, member pointers, lambdas.
 *  - Higher-order functions: map, filter, fold patterns.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
#include <string>
#include <string_view>
#include <memory>
#include <cassert>

template<typename C>
void print(std::string_view label, const C& c) {
    std::cout << "  " << label << ": [ ";
    for (const auto& v : c) std::cout << v << " ";
    std::cout << "]\n";
}

// ── 1. Function objects (functors) ───────────────────────────────────────────

// Stateless functor — equivalent to a free function
struct Multiply {
    int operator()(int a, int b) const { return a * b; }
};

// Stateful functor — carries data
struct Adder {
    int offset;
    explicit Adder(int n) : offset(n) {}
    int operator()(int x) const { return x + offset; }
};

// Functor used as a comparator
struct CaseInsensitiveLess {
    bool operator()(const std::string& a, const std::string& b) const {
        // Compare character by character, lowercased
        return std::lexicographical_compare(
            a.begin(), a.end(),
            b.begin(), b.end(),
            [](char x, char y){ return std::tolower(x) < std::tolower(y); });
    }
};

void functor_demo() {
    std::cout << "\n=== Functors ===\n";

    Multiply mul;
    std::cout << "  Multiply(3,4) = " << mul(3, 4) << "\n";

    Adder add10(10);
    std::vector<int> v = {1,2,3,4,5};
    std::transform(v.begin(), v.end(), v.begin(), add10);
    print("add10 to each", v);

    std::vector<std::string> words = {"Banana","apple","Cherry"};
    std::sort(words.begin(), words.end(), CaseInsensitiveLess{});
    print("case-insensitive sort", words);

    // STL built-in functors
    std::vector<int> u = {5,1,3,2,4};
    std::sort(u.begin(), u.end(), std::greater<int>{});
    print("std::greater sort", u);

    // Arithmetic functors
    std::cout << "  negate(7) = " << std::negate<int>{}(7) << "\n";
    std::cout << "  plus(3,4) = " << std::plus<int>{}(3,4) << "\n";
}

// ── 2. std::function ─────────────────────────────────────────────────────────

// std::function can hold any callable with a matching signature.
// Cost: heap allocation + virtual dispatch — avoid in hot paths.

int free_add(int a, int b) { return a + b; }

void std_function_demo() {
    std::cout << "\n=== std::function ===\n";

    // From a free function
    std::function<int(int,int)> f1 = free_add;
    std::cout << "  free function: " << f1(2, 3) << "\n";

    // From a lambda
    std::function<int(int,int)> f2 = [](int a, int b){ return a * b; };
    std::cout << "  lambda:        " << f2(4, 5) << "\n";

    // From a functor
    std::function<int(int,int)> f3 = Multiply{};
    std::cout << "  functor:       " << f3(6, 7) << "\n";

    // Storing heterogeneous callables in a container
    std::vector<std::function<int(int)>> ops = {
        [](int x){ return x + 1;  },
        [](int x){ return x * 2;  },
        [](int x){ return x * x;  },
    };
    int val = 3;
    for (auto& op : ops)
        val = op(val);
    std::cout << "  pipeline result: " << val << "\n";   // ((3+1)*2)^2 = 64

    // Null check
    std::function<void()> empty;
    std::cout << "  empty callable: " << std::boolalpha << !empty << "\n";
}

// ── 3. Lambdas — syntax and capture ─────────────────────────────────────────
void lambda_capture_demo() {
    std::cout << "\n=== Lambda Captures ===\n";

    int  x    = 10;
    int  y    = 20;
    std::string msg = "hello";

    // Capture by value — snapshot at creation time
    auto by_val = [x, y]{ return x + y; };
    x = 99;                             // changing x has no effect
    std::cout << "  by value (x=10,y=20 snapshot): " << by_val() << "\n";

    // Capture by reference — reflects current value
    int counter = 0;
    auto by_ref = [&counter]{ ++counter; };
    by_ref(); by_ref(); by_ref();
    std::cout << "  by ref counter: " << counter << "\n";

    // Capture all by value [=]
    auto all_val = [=]{ return msg + " world"; };
    std::cout << "  [=]: " << all_val() << "\n";

    // Capture all by reference [&]
    int sum = 0;
    std::vector<int> v = {1,2,3,4,5};
    std::for_each(v.begin(), v.end(), [&](int n){ sum += n; });
    std::cout << "  [&] sum: " << sum << "\n";

    // Mixed capture
    int base = 100;
    std::vector<int> out;
    std::transform(v.begin(), v.end(), std::back_inserter(out),
                   [base, &out](int n){ return base + n; });
    print("mixed capture", out);
}

// ── 4. mutable lambdas ────────────────────────────────────────────────────────
void mutable_lambda_demo() {
    std::cout << "\n=== mutable Lambda ===\n";

    // By default, captured-by-value variables are const inside the lambda.
    // 'mutable' removes that restriction and gives the lambda its own copy to modify.

    int count = 0;
    auto counter = [count]() mutable {
        return ++count;         // modifies the lambda's own copy, not the original
    };
    std::cout << "  calls: " << counter() << " " << counter() << " " << counter() << "\n";
    std::cout << "  original count still: " << count << "\n";   // unchanged

    // Practical use: stateful generator
    auto id_gen = [n = 0]() mutable { return n++; };
    std::vector<int> ids(5);
    std::generate(ids.begin(), ids.end(), id_gen);
    print("generated ids", ids);
}

// ── 5. Generic lambdas (C++14) ───────────────────────────────────────────────
void generic_lambda_demo() {
    std::cout << "\n=== Generic Lambdas (C++14) ===\n";

    // 'auto' parameter makes this a template operator()
    auto square = [](auto x){ return x * x; };
    std::cout << "  square(3)   = " << square(3)   << "\n";
    std::cout << "  square(2.5) = " << square(2.5) << "\n";
    std::cout << "  square('B') = " << (int)square('B') << "\n"; // 66*66

    // Generic comparator
    auto less_abs = [](auto a, auto b){ return std::abs(a) < std::abs(b); };
    std::vector<int> v = {-3, 1, -7, 2, -5};
    std::sort(v.begin(), v.end(), less_abs);
    print("sort by |x|", v);
}

// ── 6. std::bind and its modern replacements ─────────────────────────────────
int pow_int(int base, int exp) {
    int r = 1;
    for (int i = 0; i < exp; ++i) r *= base;
    return r;
}

void bind_demo() {
    std::cout << "\n=== std::bind vs Lambdas ===\n";

    // std::bind: bind arguments to a callable
    using namespace std::placeholders;  // _1, _2, ...
    auto square_bind = std::bind(pow_int, _1, 2);    // base=_1, exp=2
    std::cout << "  bind square(5) = " << square_bind(5) << "\n";

    // The modern replacement is a lambda — more readable, no overhead:
    auto square_lambda = [](int base){ return pow_int(base, 2); };
    std::cout << "  lambda square(5) = " << square_lambda(5) << "\n";

    // Reorder arguments with bind
    auto flip = std::bind(pow_int, _2, _1);           // flip args
    std::cout << "  bind flip pow(2,10)=pow(10,2) = " << flip(2, 10) << "\n";

    // std::bind still useful: binding member functions
    struct Greeter {
        std::string prefix;
        void greet(const std::string& name) const {
            std::cout << "  " << prefix << " " << name << "!\n";
        }
    };
    Greeter g{"Hello"};
    auto greet_fn = std::bind(&Greeter::greet, &g, _1);
    greet_fn("World");

    // But a lambda is clearer:
    auto greet_lambda = [&g](const std::string& name){ g.greet(name); };
    greet_lambda("C++");
}

// ── 7. Higher-order function patterns ────────────────────────────────────────

// map: transform each element
template<typename T, typename F>
std::vector<T> map_vec(std::vector<T> v, F f) {
    std::transform(v.begin(), v.end(), v.begin(), f);
    return v;
}

// filter: keep elements matching predicate
template<typename T, typename Pred>
std::vector<T> filter_vec(const std::vector<T>& v, Pred p) {
    std::vector<T> out;
    std::copy_if(v.begin(), v.end(), std::back_inserter(out), p);
    return out;
}

// fold (reduce)
template<typename T, typename U, typename F>
U fold_vec(const std::vector<T>& v, U init, F f) {
    return std::accumulate(v.begin(), v.end(), init, f);
}

void higher_order_demo() {
    std::cout << "\n=== Higher-Order Patterns ===\n";

    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    auto squares  = map_vec(v, [](int x){ return x * x; });
    auto evens    = filter_vec(v, [](int x){ return x % 2 == 0; });
    auto sum      = fold_vec(v, 0, std::plus<int>{});
    auto product  = fold_vec(v, 1, std::multiplies<int>{});

    print("squares", squares);
    print("evens",   evens);
    std::cout << "  sum     = " << sum     << "\n";
    std::cout << "  product = " << product << "\n";

    // Compose: sum of squares of even numbers
    auto even_squares = map_vec(filter_vec(v, [](int x){ return x % 2 == 0; }),
                                [](int x){ return x * x; });
    int result = fold_vec(even_squares, 0, std::plus<int>{});
    std::cout << "  sum of squares of evens = " << result << "\n";
}

// ── 8. Immediately invoked lambda (IIFE) ─────────────────────────────────────
void iife_demo() {
    std::cout << "\n=== Immediately Invoked Lambda (IIFE) ===\n";

    // Useful for initializing a const variable with complex logic
    const int config = []{
        int v = 42;
        // ... imagine complex configuration logic ...
        return v * 2;
    }();
    std::cout << "  config = " << config << "\n";

    // IIFE with capture for in-place initialization
    std::string greeting = "World";
    const std::string msg = [&greeting]{
        return "Hello, " + greeting + "!";
    }();
    std::cout << "  msg = " << msg << "\n";
}

int main() {
    functor_demo();
    std_function_demo();
    lambda_capture_demo();
    mutable_lambda_demo();
    generic_lambda_demo();
    bind_demo();
    higher_order_demo();
    iife_demo();

    std::cout << "\n✓ Lesson 10 complete.\n";
}
