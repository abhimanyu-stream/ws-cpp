/**
 * LESSON 00 — C++17 Syntax From Scratch
 * ======================================
 * This file teaches every important C++17 syntax feature
 * with simple examples, comparisons to older C++, and
 * "why it matters" explanations.
 *
 * Compile:  g++ -std=c++17 -Wall 00_cpp17_syntax_guide.cpp -o guide
 *
 * Sections:
 *  1.  auto keyword
 *  2.  Range-based for loop
 *  3.  Uniform initialization  { }
 *  4.  nullptr
 *  5.  Trailing return types
 *  6.  Lambdas
 *  7.  Move semantics (rvalue references, std::move)
 *  8.  Structured bindings          [C++17]
 *  9.  if / switch with initializer [C++17]
 * 10.  std::optional                [C++17]
 * 11.  std::variant                 [C++17]
 * 12.  std::string_view             [C++17]
 * 13.  Fold expressions             [C++17]
 * 14.  if constexpr                 [C++17]
 * 15.  Class template argument deduction (CTAD) [C++17]
 * 16.  Inline variables             [C++17]
 * 17.  Nested namespaces            [C++17]
 * 18.  [[nodiscard]], [[maybe_unused]], [[fallthrough]] [C++17]
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <optional>
#include <variant>
#include <string_view>
#include <type_traits>   // for if constexpr examples

// ═══════════════════════════════════════════════════════════════════════════════
// 1.  auto  — let the compiler deduce the type
// ═══════════════════════════════════════════════════════════════════════════════
void section_auto() {
    std::cout << "\n── 1. auto ──────────────────────────────────────\n";

    // OLD way — you repeat the type name
    int              old_i  = 42;
    double           old_d  = 3.14;
    std::string      old_s  = "hello";

    // NEW way — compiler figures it out from the right-hand side
    auto i  = 42;          // int
    auto d  = 3.14;        // double
    auto s  = std::string("hello");   // std::string
    auto f  = 3.14f;       // float  (the 'f' suffix tells compiler it's float)
    auto l  = 42L;         // long
    auto ul = 42UL;        // unsigned long

    std::cout << "i=" << i << " d=" << d << " s=" << s << "\n";

    // auto with references — add & to avoid copying
    std::string greeting = "world";
    auto& ref = greeting;      // ref is std::string&  (reference, no copy)
    ref = "C++17";             // modifies 'greeting'
    std::cout << "greeting is now: " << greeting << "\n";

    // auto with const
    const auto ci = 100;       // const int
    // ci = 200;               // ERROR — can't modify const

    // auto in a for loop — very common usage (see section 2)
    std::vector<int> nums = {1, 2, 3};
    for (auto n : nums)        // n is int (copied each time)
        std::cout << n << " ";
    std::cout << "\n";

    // WHY USE auto?
    // 1. Avoids long type names:
    //    std::vector<std::pair<int,std::string>>::iterator it = v.begin();
    //    auto it = v.begin();   ← much cleaner
    // 2. Prevents accidental type narrowing (e.g. int vs size_t)
    // 3. Future-proof — if you change a function's return type,
    //    variables using auto adapt automatically.

    (void)old_i; (void)old_d; (void)old_s; (void)ci; // suppress unused warnings
}

// ═══════════════════════════════════════════════════════════════════════════════
// 2.  Range-based for loop
// ═══════════════════════════════════════════════════════════════════════════════
void section_range_for() {
    std::cout << "\n── 2. Range-based for ───────────────────────────\n";

    std::vector<int> v = {10, 20, 30, 40};

    // OLD way
    for (int i = 0; i < (int)v.size(); ++i)
        std::cout << v[i] << " ";
    std::cout << "\n";

    // NEW way — by value (copies each element)
    for (auto x : v)
        std::cout << x << " ";
    std::cout << "\n";

    // By const reference — no copy, no modification (PREFERRED for read-only)
    for (const auto& x : v)
        std::cout << x << " ";
    std::cout << "\n";

    // By reference — can modify elements
    for (auto& x : v)
        x *= 2;    // doubles every element in-place

    // Works on arrays too
    int arr[] = {1, 2, 3};
    for (auto x : arr)
        std::cout << x << " ";
    std::cout << "\n";

    // Works on initializer lists
    for (auto x : {5, 6, 7})
        std::cout << x << " ";
    std::cout << "\n";

    // Works on std::string (iterates characters)
    for (auto c : std::string("hello"))
        std::cout << c << "-";
    std::cout << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 3.  Uniform initialization  { }   (brace initialization)
// ═══════════════════════════════════════════════════════════════════════════════
void section_brace_init() {
    std::cout << "\n── 3. Uniform initialization { } ────────────────\n";

    // Before C++11 there were 4 different init syntaxes.
    // Braces work everywhere consistently.

    int   a  = 5;      // copy initialization (old)
    int   b(5);        // direct initialization (old)
    int   c{5};        // brace initialization (new, preferred)
    int   d = {5};     // also valid

    // KEY BENEFIT: braces prevent narrowing conversions (compiler error)
    // int x = 3.9;    // silently truncates to 3
    // int x{3.9};     // ERROR — narrowing conversion not allowed

    // Initializing containers — very natural with braces
    std::vector<int>              v  = {1, 2, 3, 4, 5};
    std::map<std::string, int>    m  = { {"alice", 30}, {"bob", 25} };
    std::pair<int, std::string>   p  = {42, "hello"};

    // Struct/class initialization
    struct Point { int x; int y; };
    Point pt{10, 20};          // no constructor needed for simple structs
    Point pt2 = {30, 40};      // also fine

    std::cout << "pt = (" << pt.x << ", " << pt.y << ")\n";
    std::cout << "v[0]=" << v[0] << " m[alice]=" << m["alice"] << "\n";

    // Zero-initialization with empty braces
    int   zero_i{};    // 0
    double zero_d{};   // 0.0
    std::string zero_s{};  // ""

    std::cout << "zero_i=" << zero_i << " zero_d=" << zero_d << "\n";

    (void)a; (void)b; (void)c; (void)d; (void)p; (void)pt2;
    (void)zero_i; (void)zero_d;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 4.  nullptr  — the type-safe null pointer
// ═══════════════════════════════════════════════════════════════════════════════
void section_nullptr() {
    std::cout << "\n── 4. nullptr ───────────────────────────────────\n";

    // OLD way — NULL is just the integer 0, causes ambiguity
    // void foo(int);
    // void foo(int*);
    // foo(NULL);   // which foo? ambiguous!

    // NEW way — nullptr has type std::nullptr_t, never an integer
    int* p = nullptr;         // clearly a null pointer
    // int* q = 0;            // still works but less clear
    // int* r = NULL;         // macro, old style

    if (p == nullptr)
        std::cout << "p is null\n";

    // nullptr converts to any pointer type
    int*    ip  = nullptr;
    double* dp  = nullptr;
    char*   cp  = nullptr;

    // But NOT to int — no ambiguity
    // int x = nullptr;   // ERROR

    (void)ip; (void)dp; (void)cp;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5.  Trailing return type  ->
// ═══════════════════════════════════════════════════════════════════════════════
void section_trailing_return() {
    std::cout << "\n── 5. Trailing return type ──────────────────────\n";

    // Traditional syntax
    int add_old(int a, int b);       // declaration only shown here

    // Trailing return type syntax — return type comes AFTER parameters
    auto add = [](int a, int b) -> int {
        return a + b;
    };

    // WHY is this useful?
    // When the return type depends on the parameters, you can reference them:
    auto multiply = [](auto a, auto b) -> decltype(a * b) {
        return a * b;
    };

    std::cout << "3 + 4 = "   << add(3, 4)      << "\n";
    std::cout << "3 * 4.5 = " << multiply(3, 4.5) << "\n";   // returns double

    // Also used in regular functions when return type is complex
    // auto get_map() -> std::map<std::string, std::vector<int>>;
    // Easier to read than:
    // std::map<std::string, std::vector<int>> get_map();
}

// ═══════════════════════════════════════════════════════════════════════════════
// 6.  Lambdas — anonymous inline functions
// ═══════════════════════════════════════════════════════════════════════════════
void section_lambdas() {
    std::cout << "\n── 6. Lambdas ───────────────────────────────────\n";

    // Anatomy:  [capture](parameters) -> return_type { body }

    // Simplest lambda — no capture, no params
    auto say_hi = []() { std::cout << "Hi!\n"; };
    say_hi();

    // With parameters
    auto add = [](int a, int b) { return a + b; };
    std::cout << "add(2,3) = " << add(2, 3) << "\n";

    // ── Capture list ──────────────────────────────────────────────────────────
    int x = 10;
    int y = 20;

    // Capture by value — gets a COPY at the time the lambda is defined
    auto by_value = [x, y]() {
        std::cout << "copy: x=" << x << " y=" << y << "\n";
        // x = 999;  // ERROR — copied values are const by default
    };

    // Capture by reference — sees live variable
    auto by_ref = [&x, &y]() {
        x += 1;    // modifies the ORIGINAL x
        std::cout << "ref: x=" << x << " y=" << y << "\n";
    };

    // Capture ALL by value
    auto all_copy = [=]() { std::cout << "x=" << x << "\n"; };

    // Capture ALL by reference
    auto all_ref = [&]() { x += 100; };

    by_value();    // x=10, y=20
    by_ref();      // x becomes 11
    all_copy();    // sees x=11 (captured at this point)
    all_ref();     // x becomes 111

    // ── mutable lambda ────────────────────────────────────────────────────────
    // by default, value-captures are const inside the lambda
    // 'mutable' removes that restriction
    auto counter = [n = 0]() mutable {
        return ++n;    // modifies the captured copy, not an external variable
    };
    std::cout << counter() << " " << counter() << " " << counter() << "\n"; // 1 2 3

    // ── Lambda as function argument ───────────────────────────────────────────
    std::vector<int> v = {5, 1, 4, 2, 3};
    std::sort(v.begin(), v.end(), [](int a, int b) {
        return a < b;   // sort ascending
    });
    for (auto n : v) std::cout << n << " ";
    std::cout << "\n";

    // ── Generic lambda (C++14, common in C++17 code) ─────────────────────────
    auto print = [](auto val) { std::cout << val << "\n"; };
    print(42);
    print("hello");
    print(3.14);
}

// ═══════════════════════════════════════════════════════════════════════════════
// 7.  Move semantics — rvalue references, std::move
// ═══════════════════════════════════════════════════════════════════════════════
void section_move_semantics() {
    std::cout << "\n── 7. Move semantics ────────────────────────────\n";

    // UNDERSTANDING lvalue vs rvalue:
    //   lvalue — has a name, lives beyond the expression, addressable
    //   rvalue — temporary, no name, dies at end of expression

    int a = 5;    // 'a' is lvalue, '5' is rvalue
    // int& r = 5;    // ERROR — can't bind lvalue ref to rvalue
    int&& rr = 5;    // OK — rvalue reference binds to temporary

    // WHY does this matter for performance?
    // Copying a std::string with 1 million chars allocates + copies all chars.
    // MOVING just transfers the internal pointer — O(1) cost.

    std::string s1 = "Hello, World! This is a long string.";

    // COPY — s1 still exists, s2 gets a full duplicate
    std::string s2 = s1;
    std::cout << "after copy: s1='" << s1 << "'\n";

    // MOVE — s1's data is stolen, s1 becomes empty
    std::string s3 = std::move(s1);
    std::cout << "after move: s1='" << s1 << "' (now empty)\n";
    std::cout << "             s3='" << s3 << "'\n";

    // std::move is just a CAST — it doesn't move anything by itself.
    // It says "I no longer need this variable, feel free to steal from it."
    // The actual move happens in the move constructor/assignment operator.

    // Common usage patterns:
    // 1. Moving into a container (avoid copy of large objects)
    std::vector<std::string> words;
    std::string word = "expensive";
    words.push_back(std::move(word));  // moves 'word' into vector
    // 'word' is now in a valid-but-unspecified state

    // 2. Moving out of a function (Return Value Optimization usually does this)
    auto make_string = []() -> std::string {
        std::string result = "big result";
        return result;    // compiler moves or elides automatically (RVO)
    };
    auto result = make_string();
    std::cout << "result: " << result << "\n";

    (void)a; (void)rr; (void)s2;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 8.  Structured bindings  [C++17]  — unpack tuples, pairs, structs
// ═══════════════════════════════════════════════════════════════════════════════
void section_structured_bindings() {
    std::cout << "\n── 8. Structured bindings [C++17] ──────────────\n";

    // ── Pairs ─────────────────────────────────────────────────────────────────
    std::pair<int, std::string> p = {42, "hello"};

    // OLD way
    int         old_id   = p.first;
    std::string old_name = p.second;

    // NEW way — structured binding
    auto [id, name] = p;    // id=42, name="hello"
    std::cout << "id=" << id << " name=" << name << "\n";

    // ── Tuples ────────────────────────────────────────────────────────────────
    auto t = std::make_tuple(1, 3.14, std::string("world"));

    // OLD way — verbose
    auto old_i = std::get<0>(t);
    auto old_d = std::get<1>(t);
    auto old_s = std::get<2>(t);

    // NEW way
    auto [ti, td, ts] = t;
    std::cout << "ti=" << ti << " td=" << td << " ts=" << ts << "\n";

    // ── Structs ───────────────────────────────────────────────────────────────
    struct Color { int r, g, b; };
    Color red{255, 0, 0};

    auto [r, g, b] = red;
    std::cout << "r=" << r << " g=" << g << " b=" << b << "\n";

    // ── Map iteration — most common use case ──────────────────────────────────
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}, {"Carol", 92}};

    // OLD way
    for (auto it = scores.begin(); it != scores.end(); ++it)
        std::cout << it->first << ":" << it->second << " ";
    std::cout << "\n";

    // NEW way — much cleaner
    for (const auto& [person, score] : scores)
        std::cout << person << ":" << score << " ";
    std::cout << "\n";

    // ── By reference — can modify the original ────────────────────────────────
    for (auto& [person, score] : scores)
        score += 5;    // give everyone +5 bonus

    (void)old_id; (void)old_name; (void)old_i; (void)old_d; (void)old_s;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 9.  if / switch with initializer  [C++17]
// ═══════════════════════════════════════════════════════════════════════════════
void section_if_with_init() {
    std::cout << "\n── 9. if/switch with initializer [C++17] ────────\n";

    std::map<std::string, int> m = {{"x", 10}, {"y", 20}};

    // OLD way — 'it' leaks into the outer scope
    {
        auto it = m.find("x");
        if (it != m.end())
            std::cout << "found x=" << it->second << "\n";
        // 'it' still exists here (pollutes scope)
    }

    // NEW way — 'it' is scoped to the if block
    if (auto it = m.find("x"); it != m.end()) {
        //       ───────────── ─────────────
        //       initializer   condition
        std::cout << "found x=" << it->second << "\n";
    }   // 'it' gone here

    // Useful pattern: map::insert returns {iterator, bool}
    if (auto [iter, inserted] = m.insert({"z", 30}); inserted) {
        std::cout << "inserted z=" << iter->second << "\n";
    } else {
        std::cout << "z already existed\n";
    }

    // switch with initializer
    int get_code = 2;
    switch (auto code = get_code; code) {
        case 1:  std::cout << "one\n";   break;
        case 2:  std::cout << "two\n";   break;
        default: std::cout << "other\n"; break;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 10.  std::optional  [C++17] — a value that might not exist
// ═══════════════════════════════════════════════════════════════════════════════
void section_optional() {
    std::cout << "\n── 10. std::optional [C++17] ────────────────────\n";

    // Before optional, returning "no value" required:
    //   - returning -1 or 0 as sentinel (magic numbers, error-prone)
    //   - output parameters (ugly API)
    //   - raw pointers that might be null

    // std::optional<T> either contains a T or is empty (std::nullopt)

    auto divide = [](double a, double b) -> std::optional<double> {
        if (b == 0.0) return std::nullopt;   // no value
        return a / b;                         // has value
    };

    auto r1 = divide(10.0, 2.0);
    auto r2 = divide(10.0, 0.0);

    // Check if has value
    if (r1.has_value())
        std::cout << "10/2 = " << r1.value() << "\n";

    if (!r2)    // operator bool — same as has_value()
        std::cout << "division by zero — no result\n";

    // Safe access with default
    std::cout << "r2 or default: " << r2.value_or(-1.0) << "\n";

    // With structured binding (C++17)
    std::map<std::string, int> m = {{"age", 25}};
    auto find_val = [&](const std::string& key) -> std::optional<int> {
        if (auto it = m.find(key); it != m.end())
            return it->second;
        return std::nullopt;
    };

    if (auto age = find_val("age")) {
        std::cout << "age = " << *age << "\n";   // dereference like a pointer
    }
    if (!find_val("height")) {
        std::cout << "height not found\n";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 11.  std::variant  [C++17] — type-safe union
// ═══════════════════════════════════════════════════════════════════════════════
void section_variant() {
    std::cout << "\n── 11. std::variant [C++17] ─────────────────────\n";

    // A variant holds exactly ONE of the listed types at a time
    // Think of it as a type-safe version of C's union

    std::variant<int, double, std::string> v;

    v = 42;
    std::cout << "holds int: " << std::get<int>(v) << "\n";

    v = 3.14;
    std::cout << "holds double: " << std::get<double>(v) << "\n";

    v = std::string("hello");
    std::cout << "holds string: " << std::get<std::string>(v) << "\n";

    // Check which type is active
    std::cout << "is string? " << std::holds_alternative<std::string>(v) << "\n";
    std::cout << "is int?    " << std::holds_alternative<int>(v)         << "\n";

    // std::visit — call a lambda/visitor for whichever type is active
    // The overloaded pattern uses multiple lambdas:
    struct Visitor {
        void operator()(int i)               { std::cout << "int: "    << i << "\n"; }
        void operator()(double d)            { std::cout << "double: " << d << "\n"; }
        void operator()(const std::string& s){ std::cout << "string: " << s << "\n"; }
    };

    v = 100;
    std::visit(Visitor{}, v);   // prints "int: 100"

    v = std::string("world");
    std::visit(Visitor{}, v);   // prints "string: world"

    // Practical use: error handling without exceptions
    using Result = std::variant<int, std::string>;   // int=success, string=error
    auto safe_div = [](int a, int b) -> Result {
        if (b == 0) return std::string("division by zero");
        return a / b;
    };

    auto res = safe_div(10, 2);
    if (auto* val = std::get_if<int>(&res))
        std::cout << "result: " << *val << "\n";

    res = safe_div(10, 0);
    if (auto* err = std::get_if<std::string>(&res))
        std::cout << "error: " << *err << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 12.  std::string_view  [C++17] — non-owning view of a string
// ═══════════════════════════════════════════════════════════════════════════════
void section_string_view() {
    std::cout << "\n── 12. std::string_view [C++17] ─────────────────\n";

    // The problem: std::string always owns (and allocates) its data.
    // Passing a string to a function that only READS it wastes time/memory.

    // OLD approach — copies the string on every call
    auto old_print = [](std::string s) {       // copy!
        std::cout << s << "\n";
    };

    // Also problematic:
    auto also_bad = [](const std::string& s) { // no copy, but only accepts std::string
        std::cout << s << "\n";
        // Can't pass a const char* without implicit conversion
    };

    // GOOD — string_view is just {pointer + length}, zero allocation
    auto print_sv = [](std::string_view sv) {  // accepts ANY string-like thing
        std::cout << sv << " (length=" << sv.size() << ")\n";
    };

    std::string  s = "Hello";
    const char*  c = "World";
    char         arr[] = "Array!";

    print_sv(s);     // from std::string
    print_sv(c);     // from const char*
    print_sv(arr);   // from char[]
    print_sv("literal");  // from string literal — NO allocation at all

    // string_view operations — same as std::string but non-owning
    std::string_view sv = "Hello, World!";
    std::cout << "substr: " << sv.substr(7, 5) << "\n";   // "World"
    std::cout << "starts H? " << sv.starts_with("Hello") << "\n";  // C++20 actually
    std::cout << "find ',': " << sv.find(',') << "\n";

    // WARNING: string_view does NOT own the data
    // Never return a string_view pointing to a local string!
    // auto bad() -> std::string_view {
    //     std::string local = "oops";
    //     return local;   // DANGLING — local is destroyed!
    // }

    (void)old_print; (void)also_bad;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 13.  Fold expressions  [C++17] — variadic template magic
// ═══════════════════════════════════════════════════════════════════════════════

// A variadic template accepts any number of arguments of any types
// Fold expressions let you apply an operator across all of them

// Sum any number of values of any types
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);   // fold expression: (a1 + (a2 + (a3 + ...)))
}

// Print any number of arguments
template<typename... Args>
void print_all(Args&&... args) {
    ((std::cout << args << " "), ...);  // comma fold
    std::cout << "\n";
}

// Check if ALL values are positive
template<typename... Args>
bool all_positive(Args... args) {
    return ((args > 0) && ...);
}

void section_fold_expressions() {
    std::cout << "\n── 13. Fold expressions [C++17] ─────────────────\n";

    std::cout << sum(1, 2, 3, 4, 5) << "\n";          // 15
    std::cout << sum(1.5, 2.5, 3.0) << "\n";          // 7.0
    std::cout << sum(std::string("a"), "b", "c") << "\n"; // "abc"

    print_all(1, "hello", 3.14, 'x');

    std::cout << all_positive(1, 2, 3)    << "\n";   // 1 (true)
    std::cout << all_positive(1, -2, 3)   << "\n";   // 0 (false)
}

// ═══════════════════════════════════════════════════════════════════════════════
// 14.  if constexpr  [C++17] — compile-time branching in templates
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
void describe(T val) {
    // The right branch is chosen AT COMPILE TIME based on the type.
    // The other branch doesn't even get compiled — no errors from dead code.
    if constexpr (std::is_integral_v<T>) {
        std::cout << val << " is an integer\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << val << " is a floating-point\n";
    } else {
        std::cout << val << " is something else\n";
    }
}

template<typename T>
auto to_string_safe(T val) -> std::string {
    if constexpr (std::is_same_v<T, std::string>)
        return val;                             // already a string
    else if constexpr (std::is_arithmetic_v<T>)
        return std::to_string(val);             // convert number to string
    else
        return "(unknown type)";
}

void section_if_constexpr() {
    std::cout << "\n── 14. if constexpr [C++17] ─────────────────────\n";

    describe(42);
    describe(3.14);
    describe(std::string("hi"));

    std::cout << to_string_safe(100)        << "\n";
    std::cout << to_string_safe(3.14f)      << "\n";
    std::cout << to_string_safe(std::string("already")) << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 15.  Class Template Argument Deduction (CTAD)  [C++17]
// ═══════════════════════════════════════════════════════════════════════════════
void section_ctad() {
    std::cout << "\n── 15. CTAD [C++17] ─────────────────────────────\n";

    // Before C++17 — had to specify template arguments explicitly
    std::pair<int, std::string>   old_p(42, "hello");
    std::vector<int>              old_v{1, 2, 3};
    std::tuple<int, double, char> old_t(1, 2.0, 'a');

    // C++17 — compiler deduces template arguments from constructor args
    std::pair  p(42, std::string("hello"));   // deduced: pair<int, string>
    std::vector v{1, 2, 3};                   // deduced: vector<int>
    std::tuple  t(1, 2.0, 'a');               // deduced: tuple<int, double, char>

    std::cout << "pair: " << p.first << ", " << p.second << "\n";
    std::cout << "vector[0]: " << v[0] << "\n";

    // Practical example — locks
    // std::lock_guard<std::mutex> lock(mutex);   // old
    // std::lock_guard lock(mutex);               // C++17 CTAD

    (void)old_p; (void)old_v; (void)old_t;
}

// ═══════════════════════════════════════════════════════════════════════════════
// 16.  Nested namespaces  [C++17]
// ═══════════════════════════════════════════════════════════════════════════════

// OLD way — repeated namespace blocks
namespace old_company {
    namespace old_project {
        namespace old_module {
            void foo() { std::cout << "old nested namespace\n"; }
        }
    }
}

// NEW way — one line
namespace company::project::module {
    void foo() { std::cout << "new nested namespace\n"; }
}

void section_nested_namespaces() {
    std::cout << "\n── 16. Nested namespaces [C++17] ────────────────\n";
    old_company::old_project::old_module::foo();
    company::project::module::foo();
}

// ═══════════════════════════════════════════════════════════════════════════════
// 17.  Attributes  [C++17]:  [[nodiscard]], [[maybe_unused]], [[fallthrough]]
// ═══════════════════════════════════════════════════════════════════════════════

// [[nodiscard]] — compiler warns if return value is ignored
[[nodiscard]] int compute_important_value() {
    return 42;
}

// [[nodiscard]] on a type — warns whenever an instance is discarded
struct [[nodiscard]] ErrorCode {
    int code;
};
ErrorCode do_something() { return {0}; }

// [[maybe_unused]] — suppress "unused variable" warning
void process([[maybe_unused]] int debug_flag, int value) {
    // debug_flag might only be used in debug builds
    std::cout << "processing: " << value << "\n";
}

void section_attributes() {
    std::cout << "\n── 17. Attributes [C++17] ───────────────────────\n";

    // compute_important_value();         // WARNING: ignoring return value
    auto val = compute_important_value(); // OK
    std::cout << "val=" << val << "\n";

    // do_something();   // WARNING: ignoring ErrorCode
    auto ec = do_something();             // OK
    std::cout << "code=" << ec.code << "\n";

    // [[fallthrough]] in switch — explicit intent, no warning
    int x = 1;
    switch(x) {
        case 1:
            std::cout << "case 1\n";
            [[fallthrough]];    // intentional fallthrough — no warning
        case 2:
            std::cout << "case 1 or 2\n";
            break;
        default:
            break;
    }

    process(0, 99);
}

// ═══════════════════════════════════════════════════════════════════════════════
// BONUS: decltype — ask the compiler "what type is this expression?"
// ═══════════════════════════════════════════════════════════════════════════════
void section_decltype() {
    std::cout << "\n── BONUS: decltype ──────────────────────────────\n";

    int    a = 5;
    double b = 3.14;

    decltype(a)     x = 10;        // x is int
    decltype(a + b) y = 1.5;       // y is double (int + double → double)
    decltype(a) *   p = &a;        // p is int*

    std::cout << "x=" << x << " y=" << y << " *p=" << *p << "\n";

    // Most common use: template functions where return type depends on args
    // auto add(T a, U b) -> decltype(a + b) { return a + b; }

    // decltype(auto) — deduce exactly the type including references
    auto& ref = a;
    decltype(auto) dr = ref;    // dr is int& (auto alone would give int)
    dr = 99;
    std::cout << "a after dr=99: " << a << "\n";

    (void)p;
}

// ═══════════════════════════════════════════════════════════════════════════════
// main
// ═══════════════════════════════════════════════════════════════════════════════
int main() {
    section_auto();
    section_range_for();
    section_brace_init();
    section_nullptr();
    section_trailing_return();
    section_lambdas();
    section_move_semantics();
    section_structured_bindings();
    section_if_with_init();
    section_optional();
    section_variant();
    section_string_view();
    section_fold_expressions();
    section_if_constexpr();
    section_ctad();
    section_nested_namespaces();
    section_attributes();
    section_decltype();

    std::cout << "\n✓ C++17 syntax guide complete.\n";
    return 0;
}
