/**
 * TOPIC 12 — Modern C++ (C++11 to C++23)
 * =========================================
 * Covers key features introduced in each standard version.
 *
 * Compile (C++17): g++ -std=c++17 -Wall -Wno-unused-parameter 12_modern_cpp.cpp -o modern
 * Compile (C++20): g++ -std=c++20 -Wall -Wno-unused-parameter 12_modern_cpp.cpp -o modern
 *
 * NOTE: #include directives must be at file scope (not inside #if blocks
 *       after function definitions).  All includes are therefore at the top.
 *       Local class/struct definitions inside functions ARE allowed in C++.
 *       Template declarations (template<typename T>) are NOT allowed
 *       inside function bodies — they live at namespace scope.
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <any>
#include <string_view>
#include <filesystem>
#include <memory>
#include <functional>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <type_traits>
#include <cmath>          // std::sqrt

// C++20 headers — available when compiling with -std=c++20
#if __cplusplus >= 202002L
#  include <concepts>
#  include <ranges>
#  include <span>
#  include <bit>
#endif

// C++23 headers
#if __cplusplus >= 202302L
#  include <expected>
// #  include <print>   // not yet in all gcc builds — skip for portability
#endif

namespace fs = std::filesystem;

// ── Variable template (C++14) — MUST be at namespace scope, not inside a fn ──
template<typename T>
constexpr T pi_v = T(3.14159265358979323846L);

// ── C++20 concept — MUST be at namespace scope ────────────────────────────────
#if __cplusplus >= 202002L
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;
#endif

// ═══════════════════════════════════════════════════════════════════════════════
// C++11 Features
// ═══════════════════════════════════════════════════════════════════════════════
void section_cpp11() {
    std::cout << "\n══ C++11 Features ══\n";

    // 1. auto — type deduction from the initializer
    auto i = 42;
    auto d = 3.14;
    auto s = std::string("hello");
    std::cout << "auto: " << i << " " << d << " " << s << "\n";

    // 2. nullptr — type-safe null pointer (replaces NULL / 0)
    int* p = nullptr;
    std::cout << "nullptr: " << (p == nullptr) << "\n";
    (void)p;

    // 3. Range-based for loop
    std::vector<int> v = {1, 2, 3, 4, 5};
    for (const auto& x : v) std::cout << x << " ";
    std::cout << "\n";

    // 4. Lambda expressions  [capture](params) { body }
    auto square = [](int x) { return x * x; };
    std::cout << "lambda square(5): " << square(5) << "\n";

    // 5. Move semantics + rvalue references
    std::string a = "hello";
    std::string b = std::move(a);  // O(1) steal — a becomes empty
    std::cout << "moved: a='" << a << "' b='" << b << "'\n";

    // 6. Smart pointers
    auto up = std::make_unique<int>(42);
    auto sp = std::make_shared<int>(99);
    std::cout << "smart ptrs: " << *up << " " << *sp << "\n";

    // 7. Uniform initialization  { }
    std::vector<int>          v2{10, 20, 30};
    std::map<std::string,int> m{{"a", 1}, {"b", 2}};
    std::cout << "brace init: v2[1]=" << v2[1] << " m[a]=" << m["a"] << "\n";

    // 8. Variadic templates + fold expression (fold is C++17 syntax but demo here)
    auto sum_all = [](auto... args) { return (args + ...); };
    std::cout << "variadic sum(1..5): " << sum_all(1, 2, 3, 4, 5) << "\n";

    // 9. constexpr — compile-time constant
    constexpr int ct = 2 * 3 * 7;
    std::cout << "constexpr 2*3*7: " << ct << "\n";

    // 10. decltype — query the type of an expression without evaluating it
    decltype(d + i) result = d + i;
    std::cout << "decltype(d+i): " << result << "\n";

    // 11. Strongly-typed enum class — no implicit int conversion
    enum class Color { Red, Green, Blue };
    Color c = Color::Green;
    // int x = c;   // ERROR — good: prevents accidental int mixing
    std::cout << "enum class Green: " << static_cast<int>(c) << "\n";

    // 12. override / final on virtual functions
    struct Base  { virtual void foo() const {} virtual ~Base() = default; };
    struct Child : Base { void foo() const override {} };  // compiler verifies override

    // 13. = default / = delete
    struct NonCopyable {
        NonCopyable()                        = default;
        NonCopyable(const NonCopyable&)      = delete;  // explicitly disabled
        NonCopyable& operator=(const NonCopyable&) = delete;
    };

    // 14. static_assert — compile-time assertion (fires if condition is false)
    static_assert(sizeof(int) >= 4, "int must be at least 4 bytes");
    std::cout << "static_assert: passed\n";

    // 15. User-defined literals
    using namespace std::literals;
    auto str = "hello"s;              // std::string — not const char*
    auto dur = std::chrono::milliseconds(100);
    std::cout << "UDL string: " << str << "\n";
    (void)dur;
}

// ═══════════════════════════════════════════════════════════════════════════════
// C++14 Features
// ═══════════════════════════════════════════════════════════════════════════════
void section_cpp14() {
    std::cout << "\n══ C++14 Features ══\n";

    // 1. Generic lambdas — 'auto' parameter (each call generates a new instantiation)
    auto print_any = [](auto val) { std::cout << val << "\n"; };
    print_any(42);
    print_any("hello");
    print_any(3.14);

    // 2. make_unique — was accidentally omitted from C++11
    auto up = std::make_unique<std::string>("C++14");
    std::cout << "make_unique: " << *up << "\n";

    // 3. Relaxed constexpr — C++11 only allowed a single return statement
    //    C++14 allows loops, if/else, local variables
    constexpr auto factorial = [](int n) constexpr {
        int result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        return result;
    };
    std::cout << "constexpr factorial(6): " << factorial(6) << "\n";

    // 4. Variable templates — must be at namespace scope (defined above main)
    //    See: template<typename T> constexpr T pi_v = T(3.14159...);
    std::cout << "pi_v<float>:  " << pi_v<float>       << "\n";
    std::cout << "pi_v<double>: " << pi_v<double>      << "\n";

    // 5. Return type deduction — 'auto' return type, compiler deduces from return
    auto get_pair = []() { return std::make_pair(1, "one"); };
    auto [n, w]   = get_pair();
    std::cout << "return deduction: " << n << " " << w << "\n";

    // 6. [[deprecated]] attribute — triggers a compiler warning at call site
    //    Declared here as a lambda for demo (normally on a function/class)
    [[deprecated("use new_func() instead")]]
    auto old_func = []() { return 0; };
    // old_func();   // would emit: warning: 'old_func' is deprecated
    (void)old_func;

    // 7. Binary literals  0b...  and digit separators  '
    int flags  = 0b1010'1100;   // clearer than 0xAC or 172
    int big    = 1'000'000;     // clearer than 1000000
    std::cout << "binary 0b10101100 = " << flags << "  million: " << big << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// C++17 Features
// ═══════════════════════════════════════════════════════════════════════════════
void section_cpp17() {
    std::cout << "\n══ C++17 Features ══\n";

    // 1. Structured bindings — unpack pairs, tuples, structs
    std::map<std::string,int> scores{{"Alice",95},{"Bob",87}};
    for (const auto& [name, score] : scores)
        std::cout << name << ":" << score << " ";
    std::cout << "\n";

    auto pair = std::make_pair(42, "hello");
    auto [num, str] = pair;
    std::cout << "structured binding: " << num << " " << str << "\n";

    // 2. if with initializer — variable scoped to the if block
    if (auto it = scores.find("Alice"); it != scores.end())
        std::cout << "Alice found: " << it->second << "\n";

    // 3. if constexpr — compile-time branching inside templates/lambdas
    auto describe = [](auto val) {
        if constexpr (std::is_integral_v<decltype(val)>)
            std::cout << val << " is integral\n";
        else
            std::cout << val << " is NOT integral\n";
    };
    describe(42);
    describe(3.14);

    // 4. std::optional — represents a value that might not exist
    auto safe_sqrt = [](double x) -> std::optional<double> {
        if (x < 0) return std::nullopt;
        return std::sqrt(x);
    };
    if (auto r = safe_sqrt(16.0)) std::cout << "sqrt(16) = " << *r << "\n";
    if (!safe_sqrt(-1.0))         std::cout << "sqrt(-1) = nullopt\n";
    std::cout << "or default: " << safe_sqrt(-1.0).value_or(-1.0) << "\n";

    // 5. std::variant — type-safe union, holds one of listed types
    std::variant<int, std::string, double> v;
    v = std::string("hello variant");
    std::visit([](auto&& val){ std::cout << "variant: " << val << "\n"; }, v);

    // 6. std::any — holds any type, type-erased
    std::any a = 42;
    std::cout << "any int: " << std::any_cast<int>(a) << "\n";
    a = std::string("now a string");
    std::cout << "any str: " << std::any_cast<std::string>(a) << "\n";

    // 7. std::string_view — non-owning read-only string reference, zero allocation
    auto count_vowels = [](std::string_view sv) {
        int cnt = 0;
        for (char c : sv)
            if (std::string_view("aeiouAEIOU").find(c) != std::string_view::npos) ++cnt;
        return cnt;
    };
    std::cout << "vowels in 'hello': " << count_vowels("hello") << "\n";

    // 8. Class Template Argument Deduction (CTAD)
    std::pair   p(42, 3.14);    // deduced: pair<int,double>  — no need to spell it out
    std::vector vv{1, 2, 3, 4}; // deduced: vector<int>
    std::cout << "CTAD pair: " << p.first << " " << p.second << "\n";
    (void)vv;

    // 9. [[nodiscard]] / [[maybe_unused]] / [[fallthrough]]
    [[maybe_unused]] int unused = 42;  // suppresses -Wunused warning

    // 10. std::filesystem — portable filesystem operations
    std::cout << "current_path: " << fs::current_path() << "\n";

    // 11. Guaranteed copy elision — returning a prvalue is ZERO copies
    auto make_str = []() -> std::string {
        return std::string("guaranteed RVO — zero copies");
    };
    auto s = make_str();
    std::cout << s << "\n";

    // 12. std::size() — works on raw arrays AND containers
    int arr[] = {1, 2, 3, 4, 5};
    std::cout << "std::size(arr): " << std::size(arr) << "\n";

    // 13. Fold expressions — apply operator across variadic pack
    auto product = [](auto... args) { return (args * ...); };
    std::cout << "product(1,2,3,4,5): " << product(1,2,3,4,5) << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// C++20 Features
// ═══════════════════════════════════════════════════════════════════════════════
#if __cplusplus >= 202002L

void section_cpp20() {
    std::cout << "\n══ C++20 Features ══\n";

    // 1. Concepts — named compile-time constraints on template parameters
    //    'Numeric' concept is defined at namespace scope above main
    auto add_nums = []<Numeric T>(T a, T b) { return a + b; };
    std::cout << "concept add(3,4):   " << add_nums(3,4)     << "\n";
    std::cout << "concept add(1.5,2): " << add_nums(1.5, 2.0)<< "\n";

    // 2. Ranges — composable, lazy algorithms
    std::vector<int> v = {5, 3, 1, 4, 2};
    std::ranges::sort(v);
    std::cout << "ranges::sort: ";
    for (auto x : v) std::cout << x << " ";
    std::cout << "\n";

    // Range views — piped, lazy (no copies until consumed)
    auto even_squares = v
        | std::views::filter([](int x){ return x % 2 == 0; })
        | std::views::transform([](int x){ return x * x; });
    std::cout << "even squares: ";
    for (auto x : even_squares) std::cout << x << " ";
    std::cout << "\n";

    // 3. std::span — non-owning view of contiguous data
    int arr[] = {10, 20, 30, 40, 50};
    std::span<int> sp(arr);
    std::cout << "span front=" << sp.front() << " back=" << sp.back() << "\n";
    std::span<int> sub = sp.subspan(1, 3);  // view of arr[1..3], no copy
    std::cout << "sub-span: ";
    for (auto x : sub) std::cout << x << " ";
    std::cout << "\n";

    // 4. Spaceship operator <=> (three-way comparison)
    int a = 5, b = 7;
    auto cmp = a <=> b;
    if (cmp < 0)  std::cout << "a < b\n";
    if (cmp == 0) std::cout << "a == b\n";
    if (cmp > 0)  std::cout << "a > b\n";

    // 5. Designated initializers — name fields explicitly
    struct Point3D { int x, y, z; };
    Point3D p{.x = 1, .y = 2, .z = 3};
    std::cout << "designated: " << p.x << " " << p.y << " " << p.z << "\n";

    // 6. Bit operations — std::popcount, std::has_single_bit, std::countl_zero
    std::cout << "popcount(0b10101100):  " << std::popcount(0b10101100u) << "\n";  // 4
    std::cout << "has_single_bit(16):   " << std::has_single_bit(16u)   << "\n";  // true
    std::cout << "bit_width(100):       " << std::bit_width(100u)       << "\n";  // 7

    // 7. Abbreviated function template — 'auto' parameter is implicit template
    auto max_auto = [](auto a, auto b) { return a > b ? a : b; };
    std::cout << "auto template max(3,7): " << max_auto(3, 7) << "\n";

    // 8. requires expression — inline constraint check
    auto print_if_numeric = [](auto val) {
        if constexpr (requires { val + val; })   // check if + is defined
            std::cout << "addable: " << val + val << "\n";
        else
            std::cout << "not addable\n";
    };
    print_if_numeric(21);
    print_if_numeric(std::string("hi"));
}

#endif   // C++20

// ═══════════════════════════════════════════════════════════════════════════════
// C++23 Features
// ═══════════════════════════════════════════════════════════════════════════════
#if __cplusplus >= 202302L

void section_cpp23() {
    std::cout << "\n══ C++23 Features ══\n";

    // 1. std::expected<T,E> — return value OR error without exceptions
    //    Cleaner alternative to error codes / exceptions for expected failures
    auto parse_int = [](std::string_view sv) -> std::expected<int, std::string> {
        try {
            return std::stoi(std::string(sv));
        } catch (...) {
            return std::unexpected(std::string("not a number: ") + std::string(sv));
        }
    };

    if (auto r = parse_int("42");  r.has_value())  std::cout << "parsed: " << *r << "\n";
    if (auto r = parse_int("abc"); !r.has_value())  std::cout << "error: "  << r.error() << "\n";

    // Chaining with and_then / or_else
    auto result = parse_int("10")
        .and_then([](int v) -> std::expected<int,std::string> { return v * 2; });
    std::cout << "and_then(*2): " << *result << "\n";

    // 2. Deducing this — explicit object parameter (self)
    struct Counter {
        int value = 0;
        // 'this' deduced — works for both lvalue and rvalue, const and non-const
        auto& increment(this auto& self) { ++self.value; return self; }
        void  print(this const auto& self) { std::cout << "count=" << self.value << "\n"; }
    };
    Counter c;
    c.increment().increment().increment().print();

    // 3. std::ranges improvements — zip, chunk, slide, etc.
    std::vector<int> a = {1, 2, 3};
    std::vector<std::string> b = {"one", "two", "three"};
    for (auto [n, w] : std::views::zip(a, b))
        std::cout << n << "=" << w << " ";
    std::cout << "\n";
}

#endif   // C++23

// ═══════════════════════════════════════════════════════════════════════════════
// main
// ═══════════════════════════════════════════════════════════════════════════════
int main() {
    std::cout << "╔══ Topic 12: Modern C++ (C++11–C++23) ══╗\n";

    section_cpp11();
    section_cpp14();
    section_cpp17();

#if __cplusplus >= 202002L
    section_cpp20();
#else
    std::cout << "\n══ C++20 — compile with -std=c++20 to enable ══\n";
#endif

#if __cplusplus >= 202302L
    section_cpp23();
#else
    std::cout << "══ C++23 — compile with -std=c++23 to enable ══\n";
#endif

    std::cout << "\n✓ Topic 12 complete.\n";
}
