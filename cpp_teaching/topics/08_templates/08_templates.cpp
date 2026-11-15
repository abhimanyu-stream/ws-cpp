/**
 * TOPIC 08 — C++ Templates — Complete Coverage
 * ==============================================
 * Covers: function/class templates, all parameter types,
 *         specialization, SFINAE, type traits, variadic,
 *         fold expressions, CRTP, concepts (C++20), if constexpr
 *
 * Compile: g++ -std=c++17 -Wall 08_templates.cpp -o templates
 *      or: g++ -std=c++20 -Wall 08_templates.cpp -o templates  (for concepts)
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <type_traits>
#include <concepts>      // C++20 — comment out if on C++17

// ═══════════════════════════════════════════════════════════════════════════════
// 1.  Function Templates
// ═══════════════════════════════════════════════════════════════════════════════

// Basic function template — T is a type parameter
template<typename T>
T max_val(T a, T b) {
    return a > b ? a : b;
}

// Multiple type parameters
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// Template with non-type parameter
template<typename T, int N>
T nth_power(T base) {
    T result = 1;
    for (int i = 0; i < N; ++i) result *= base;
    return result;
}

void section_function_templates() {
    std::cout << "\n=== Function Templates ===\n";

    // Template argument deduction — compiler deduces T from arguments
    std::cout << "max(3,7):       " << max_val(3, 7)         << "\n";   // T=int
    std::cout << "max(3.1,2.9):   " << max_val(3.1, 2.9)     << "\n";   // T=double
    std::cout << "max(a,b):       " << max_val('a', 'b')     << "\n";   // T=char

    // Explicit template argument — force a type
    std::cout << "max<double>(3,4.5): " << max_val<double>(3, 4.5) << "\n";

    std::cout << "add(3, 4.5): "  << add(3, 4.5)  << "\n";   // int+double=double
    std::cout << "2^8: "          << nth_power<int,8>(2) << "\n";  // 256
}

// ── Function Template Specialization ──────────────────────────────────────────
// Override the template for a specific type.
template<typename T>
bool is_zero(T val) { return val == T(0); }

// Full specialization for const char*
template<>
bool is_zero<const char*>(const char* val) {
    return val == nullptr || val[0] == '\0';
}

// ═══════════════════════════════════════════════════════════════════════════════
// 2.  Class Templates
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class Stack {
public:
    void push(const T& val) { data_.push_back(val); }
    void push(T&& val)      { data_.push_back(std::move(val)); }

    void pop() {
        if (!empty()) data_.pop_back();
    }

    const T& top() const { return data_.back(); }
    bool    empty() const { return data_.empty(); }
    size_t  size()  const { return data_.size(); }

    // Template member function inside a class template
    template<typename U>
    void push_as(U val) { data_.push_back(static_cast<T>(val)); }

private:
    std::vector<T> data_;
};

// Class template with default parameters
template<typename T, typename Container = std::vector<T>>
class Queue {
public:
    void enqueue(const T& v) { data_.push_back(v); }
    void dequeue()           { if (!data_.empty()) data_.erase(data_.begin()); }
    T&   front()             { return data_.front(); }
    bool empty() const       { return data_.empty(); }
private:
    Container data_;
};

void section_class_templates() {
    std::cout << "\n=== Class Templates ===\n";

    Stack<int> si;
    si.push(10); si.push(20); si.push(30);
    std::cout << "top: " << si.top() << " size: " << si.size() << "\n";
    si.pop();
    std::cout << "after pop: " << si.top() << "\n";

    Stack<std::string> ss;
    ss.push("hello");
    ss.push("world");
    std::cout << "string stack top: " << ss.top() << "\n";

    si.push_as(3.9);   // pushed as int (3)
    std::cout << "push_as(3.9) → " << si.top() << "\n";
}

// ── Partial Specialization ────────────────────────────────────────────────────
// Specialize for a family of types (e.g., all pointers).
template<typename T>
class TypeInfo {
public:
    static void describe() { std::cout << "generic type\n"; }
};

template<typename T>
class TypeInfo<T*> {
public:
    static void describe() { std::cout << "pointer type\n"; }
};

template<typename T>
class TypeInfo<T&> {
public:
    static void describe() { std::cout << "reference type\n"; }
};

// Full specialization for int
template<>
class TypeInfo<int> {
public:
    static void describe() { std::cout << "int type\n"; }
};

void section_specialization() {
    std::cout << "\n=== Specialization ===\n";
    TypeInfo<double>::describe();   // generic
    TypeInfo<int>::describe();      // int
    TypeInfo<int*>::describe();     // pointer
    TypeInfo<int&>::describe();     // reference
}

// ═══════════════════════════════════════════════════════════════════════════════
// 3.  Template Template Parameters
// ═══════════════════════════════════════════════════════════════════════════════

// A template that takes another template as a parameter
template<typename T, template<typename, typename> class Container, typename Alloc = std::allocator<T>>
class Wrapper {
public:
    Container<T, Alloc> data;
    void add(const T& v) { data.push_back(v); }
    size_t size() const  { return data.size(); }
};

void section_template_template() {
    std::cout << "\n=== Template Template Parameters ===\n";
    Wrapper<int, std::vector> w;
    w.add(1); w.add(2); w.add(3);
    std::cout << "wrapper size: " << w.size() << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 4.  Variadic Templates
// ═══════════════════════════════════════════════════════════════════════════════

// Recursive variadic template — C++11 style
template<typename T>
T sum_v(T t) { return t; }   // base case

template<typename T, typename... Rest>
T sum_v(T first, Rest... rest) {
    return first + sum_v(rest...);   // recursive call with remaining args
}

// Fold expression — C++17 style (much simpler)
template<typename... Args>
auto sum_fold(Args... args) {
    return (args + ...);   // unary right fold: (a1 + (a2 + (a3 + ...)))
}

template<typename... Args>
void print_all(Args&&... args) {
    // Comma fold — calls operator<< for each
    ((std::cout << args << " "), ...);
    std::cout << "\n";
}

template<typename... Args>
bool all_positive(Args... args) {
    return ((args > 0) && ...);   // fold with &&
}

void section_variadic() {
    std::cout << "\n=== Variadic Templates ===\n";
    std::cout << "sum_v(1,2,3,4): "    << sum_v(1,2,3,4) << "\n";
    std::cout << "sum_fold(1,2,3,4): " << sum_fold(1,2,3,4) << "\n";
    print_all(42, "hello", 3.14, true);
    std::cout << "all_positive(1,2,3): " << all_positive(1,2,3) << "\n";
    std::cout << "all_positive(1,-1,3): "<< all_positive(1,-1,3) << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5.  SFINAE — Substitution Failure Is Not An Error
// ═══════════════════════════════════════════════════════════════════════════════
// If template substitution fails, that overload is silently removed
// instead of causing a compile error. Used to select overloads based on type.

// Enable this overload only for integral types
template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
doubled(T x) {
    std::cout << "(integral) ";
    return x * 2;
}

// Enable this overload only for floating-point types
template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
doubled(T x) {
    std::cout << "(float) ";
    return x * 2.0;
}

// C++17 cleaner with _v and inline if:
template<typename T>
auto doubled_v2(T x) {
    if constexpr (std::is_integral_v<T>)
        return x * 2;
    else
        return x * 2.0;
}

void section_sfinae() {
    std::cout << "\n=== SFINAE & enable_if ===\n";
    std::cout << doubled(5)    << "\n";   // integral version
    std::cout << doubled(5.0)  << "\n";   // float version
    std::cout << doubled_v2(7) << "\n";   // if constexpr version
}

// ═══════════════════════════════════════════════════════════════════════════════
// 6.  Type Traits
// ═══════════════════════════════════════════════════════════════════════════════
void section_type_traits() {
    std::cout << "\n=== Type Traits ===\n";

    std::cout << "is_integral<int>:          " << std::is_integral_v<int>          << "\n";
    std::cout << "is_integral<double>:       " << std::is_integral_v<double>       << "\n";
    std::cout << "is_floating_point<float>:  " << std::is_floating_point_v<float>  << "\n";
    std::cout << "is_pointer<int*>:          " << std::is_pointer_v<int*>          << "\n";
    std::cout << "is_reference<int&>:        " << std::is_reference_v<int&>        << "\n";
    std::cout << "is_const<const int>:       " << std::is_const_v<const int>       << "\n";
    std::cout << "is_same<int,int>:          " << std::is_same_v<int,int>          << "\n";
    std::cout << "is_same<int,long>:         " << std::is_same_v<int,long>         << "\n";

    // Type transformations
    using T  = std::remove_const_t<const int>;    // int
    using U  = std::add_pointer_t<int>;            // int*
    using V  = std::remove_reference_t<int&>;      // int
    using W  = std::decay_t<const int&>;           // int (remove ref + const)

    std::cout << "remove_const<const int> == int: " << std::is_same_v<T, int> << "\n";
    std::cout << "add_pointer<int> == int*:        " << std::is_same_v<U, int*> << "\n";
    std::cout << "decay<const int&> == int:        " << std::is_same_v<W, int> << "\n";

    // std::conditional — choose type at compile time
    using LargerType = std::conditional_t<(sizeof(int) > sizeof(char)), int, char>;
    std::cout << "conditional int>char → int: " << std::is_same_v<LargerType, int> << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 7.  if constexpr  (C++17)
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
void describe_type(T val) {
    // The correct branch is chosen at COMPILE TIME based on the type.
    // The other branch is not compiled — avoids errors from dead code.
    if constexpr (std::is_integral_v<T>) {
        std::cout << val << " is integral, doubled=" << val*2 << "\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << val << " is float, squared=" << val*val << "\n";
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "'" << val << "' is string, len=" << val.length() << "\n";
    } else {
        std::cout << "unknown type\n";
    }
}

void section_if_constexpr() {
    std::cout << "\n=== if constexpr ===\n";
    describe_type(42);
    describe_type(3.14);
    describe_type(std::string("hello"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// 8.  CRTP — Curiously Recurring Template Pattern
// ═══════════════════════════════════════════════════════════════════════════════
// A base class that is templated on its derived class.
// Used to implement static polymorphism (compile-time, no vtable).

template<typename Derived>
class Printable {
public:
    void print() const {
        // Call the derived class's to_string() at compile time — no virtual dispatch
        std::cout << static_cast<const Derived*>(this)->to_string() << "\n";
    }
};

template<typename Derived>
class Comparable {
public:
    bool operator!=(const Derived& other) const {
        return !static_cast<const Derived*>(this)->operator==(other);
    }
    bool operator>(const Derived& other) const {
        return other < *static_cast<const Derived*>(this);
    }
};

class MyInt : public Printable<MyInt>, public Comparable<MyInt> {
public:
    explicit MyInt(int v) : v_(v) {}
    std::string to_string() const { return "MyInt(" + std::to_string(v_) + ")"; }
    bool operator==(const MyInt& o) const { return v_ == o.v_; }
    bool operator< (const MyInt& o) const { return v_ <  o.v_; }
private:
    int v_;
};

void section_crtp() {
    std::cout << "\n=== CRTP ===\n";
    MyInt a(10), b(20);
    a.print();
    b.print();
    std::cout << "a!=b: " << (a!=b) << "\n";
    std::cout << "b>a:  " << (b>a)  << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 9.  Concepts (C++20)
// ═══════════════════════════════════════════════════════════════════════════════
#if __cplusplus >= 202002L

// Define a concept — a named set of constraints
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

template<typename T>
concept Printable_c = requires(T val) {
    { std::cout << val } -> std::same_as<std::ostream&>;
};

// Use concept as template parameter constraint
template<Numeric T>
T safe_sqrt(T x) {
    if (x < 0) return T(0);
    return static_cast<T>(std::sqrt(static_cast<double>(x)));
}

// Abbreviated function template syntax
auto sum_numbers(Numeric auto a, Numeric auto b) {
    return a + b;
}

// requires clause
template<typename T>
requires Addable<T> && Printable_c<T>
void add_and_print(T a, T b) {
    std::cout << a + b << "\n";
}

void section_concepts() {
    std::cout << "\n=== Concepts (C++20) ===\n";
    std::cout << "safe_sqrt(16): "    << safe_sqrt(16)    << "\n";
    std::cout << "safe_sqrt(-4): "    << safe_sqrt(-4)    << "\n";
    std::cout << "sum_numbers(3,4.5):"<< sum_numbers(3,4.5) << "\n";
    add_and_print(10, 20);
    add_and_print(std::string("foo"), std::string("bar"));
}

#endif

int main() {
    std::cout << "╔══ Topic 08: Templates ══╗\n";

    section_function_templates();
    std::cout << "\n=== Specialization demo ===\n";
    std::cout << "is_zero(0): "   << is_zero(0)     << "\n";
    std::cout << "is_zero(0.0): " << is_zero(0.0)   << "\n";
    std::cout << "is_zero(''): "  << is_zero<const char*>("") << "\n";

    section_class_templates();
    section_specialization();
    section_template_template();
    section_variadic();
    section_sfinae();
    section_type_traits();
    section_if_constexpr();
    section_crtp();

#if __cplusplus >= 202002L
    section_concepts();
#else
    std::cout << "\n=== Concepts (C++20 — skipped) ===\n";
#endif

    std::cout << "\n✓ Topic 08 complete.\n";
}
