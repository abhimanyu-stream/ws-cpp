/**
 * TOPIC 04 — Function Pointers — Advanced
 * =========================================
 * Covers: declaration, initialization, calling, as arguments/return,
 *         arrays of function pointers, member function pointers,
 *         callbacks, vs lambda/std::function/std::bind
 *
 * Compile: g++ -std=c++17 -Wall 04_function_pointers.cpp -o func_ptrs
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>   // std::function, std::bind
#include <algorithm>    // std::transform

// ── Basic Function Pointers ───────────────────────────────────────────────────
int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

void section_basics() {
    std::cout << "\n=== Function Pointer Basics ===\n";

    // Declaration:  return_type (*name)(param_types)
    int (*op)(int, int);    // declares op as pointer to function (int,int)->int

    op = add;               // assign — no & needed (function decays to pointer)
    std::cout << "op(3,4) via add: " << op(3, 4) << "\n";   // 7

    op = sub;
    std::cout << "op(3,4) via sub: " << op(3, 4) << "\n";   // -1

    op = &mul;              // & is optional — both forms are identical
    std::cout << "op(3,4) via mul: " << op(3, 4) << "\n";   // 12

    // Calling: either  op(args)  or  (*op)(args)
    std::cout << "(*op)(3,4): " << (*op)(3, 4) << "\n";     // 12

    // typedef — make function pointer type readable
    typedef int (*BinOp)(int, int);
    BinOp fn = add;
    std::cout << "typedef BinOp: " << fn(10, 5) << "\n";    // 15

    // using (modern alias — preferred)
    using BinaryOp = int(*)(int, int);
    BinaryOp fn2 = sub;
    std::cout << "using alias: " << fn2(10, 5) << "\n";     // 5
}

// ── Function Pointer as Argument (Callback) ───────────────────────────────────
// The classic callback pattern — pass behavior into a function.
void apply_to_all(std::vector<int>& v, int (*transform)(int)) {
    for (auto& x : v) x = transform(x);
}

int square(int x)  { return x * x; }
int negate_v(int x){ return -x; }

void section_as_argument() {
    std::cout << "\n=== Function Pointer as Argument ===\n";

    std::vector<int> v = {1, 2, 3, 4, 5};

    apply_to_all(v, square);
    for (auto x : v) std::cout << x << " ";   // 1 4 9 16 25
    std::cout << "\n";

    apply_to_all(v, negate_v);
    for (auto x : v) std::cout << x << " ";   // -1 -4 -9 -16 -25
    std::cout << "\n";
}

// ── Function Pointer as Return Type ──────────────────────────────────────────
using MathOp = int(*)(int, int);

// Returns the right operation based on a character
MathOp get_operation(char op) {
    switch (op) {
        case '+': return add;
        case '-': return sub;
        case '*': return mul;
        default:  return nullptr;
    }
}

void section_as_return() {
    std::cout << "\n=== Function Pointer as Return ===\n";

    for (char c : {'+', '-', '*'}) {
        auto fn = get_operation(c);
        if (fn)
            std::cout << "10 " << c << " 3 = " << fn(10, 3) << "\n";
    }
}

// ── Array of Function Pointers ────────────────────────────────────────────────
// Useful for dispatch tables (like a switch without switch).
void section_array() {
    std::cout << "\n=== Array of Function Pointers ===\n";

    // Dispatch table — index maps to function
    int (*ops[3])(int, int) = { add, sub, mul };
    const char* names[]     = { "add", "sub", "mul" };

    for (int i = 0; i < 3; ++i)
        std::cout << names[i] << "(10,3) = " << ops[i](10, 3) << "\n";
}

// ── Member Function Pointers ──────────────────────────────────────────────────
// Pointers to methods of a class — syntax is trickier.
struct Calculator {
    int value = 0;

    void set(int v)     { value = v; }
    int  doubled()const { return value * 2; }
    int  tripled()const { return value * 3; }
    void print()  const { std::cout << "value = " << value << "\n"; }

    static int static_square(int x) { return x * x; }  // no 'this'
};

void section_member_ptrs() {
    std::cout << "\n=== Member Function Pointers ===\n";

    Calculator c;
    c.set(10);

    // Pointer to NON-STATIC member function
    // Syntax: ReturnType (ClassName::*ptr_name)(params)
    int (Calculator::*getter)() const = &Calculator::doubled;
    //   ──────────────────             ─────────────────────
    //   class qualifier                must use & and full name

    // Call: must use object + .* or ->*
    std::cout << "doubled: " << (c.*getter)() << "\n";   // 20

    getter = &Calculator::tripled;
    std::cout << "tripled: " << (c.*getter)() << "\n";   // 30

    // Via pointer to object
    Calculator* cp = &c;
    std::cout << "via ptr: " << (cp->*getter)() << "\n"; // 30

    // Pointer to STATIC member — same as a regular function pointer
    int (*static_fn)(int) = &Calculator::static_square;
    std::cout << "static square(5): " << static_fn(5) << "\n";   // 25
}

// ── Function Pointers with Structs ────────────────────────────────────────────
// Struct with function pointer member — simple "vtable by hand".
struct Shape {
    std::string name;
    double      dim;
    double (*area)(double);    // function pointer member

    double get_area() const { return area(dim); }
};

double circle_area(double r)  { return 3.14159 * r * r; }
double square_area(double s)  { return s * s; }

void section_struct_ptrs() {
    std::cout << "\n=== Function Pointers in Structs ===\n";

    Shape shapes[] = {
        {"circle", 5.0,  circle_area},
        {"square", 4.0,  square_area}
    };

    for (const auto& s : shapes)
        std::cout << s.name << " area: " << s.get_area() << "\n";
}

// ── Function Pointers vs Lambda vs std::function vs std::bind ─────────────────
void section_comparison() {
    std::cout << "\n=== Comparison: fp / lambda / std::function / std::bind ===\n";

    // 1. Raw function pointer — zero overhead, only plain functions & static methods
    int (*fp)(int, int) = add;
    std::cout << "raw fp: " << fp(2, 3) << "\n";

    // 2. Lambda — inline, can capture, type is unique/anonymous
    auto lam = [](int a, int b) { return a + b; };
    std::cout << "lambda: " << lam(2, 3) << "\n";

    // A lambda with no capture can convert to a raw function pointer
    int (*from_lambda)(int, int) = [](int a, int b) { return a + b; };
    std::cout << "lambda→fp: " << from_lambda(2, 3) << "\n";

    // 3. std::function — type-erased callable, holds anything
    //    (function pointer, lambda, functor, bind expression)
    //    Slight overhead due to type erasure
    std::function<int(int, int)> fn = add;
    fn = lam;   // can reassign to different callable types
    fn = [](int a, int b) { return a * b; };
    std::cout << "std::function: " << fn(4, 5) << "\n";   // 20

    // 4. std::bind — binds arguments to a callable, creates a new callable
    //    Largely superseded by lambdas in modern C++
    using namespace std::placeholders;
    auto add10 = std::bind(add, 10, _1);  // first arg fixed to 10
    std::cout << "bind add10(5): " << add10(5) << "\n";   // 15

    auto add_xy = std::bind(add, _2, _1); // swap argument order
    std::cout << "bind swap(3,7): " << add_xy(3, 7) << "\n";  // 10 (7+3)

    // WHEN TO USE WHAT:
    // Raw fn ptr   — C interop, callbacks to C libraries, zero overhead
    // Lambda       — preferred for inline callbacks, capturing context
    // std::function— store mixed callables, callbacks in class members
    // std::bind    — rarely; lambdas are clearer
}

// ── Callbacks and Event Handling ──────────────────────────────────────────────
// A realistic pattern: an EventSystem that holds callbacks.
class EventSystem {
public:
    using Handler = std::function<void(const std::string&)>;

    void on(const std::string& event, Handler h) {
        handlers_[event].push_back(h);
    }

    void emit(const std::string& event, const std::string& data) {
        auto it = handlers_.find(event);
        if (it != handlers_.end())
            for (auto& h : it->second) h(data);
    }

private:
    std::map<std::string, std::vector<Handler>> handlers_;
};

#include <map>

void section_callbacks() {
    std::cout << "\n=== Callbacks / Event Handling ===\n";

    EventSystem ev;

    ev.on("click", [](const std::string& d) {
        std::cout << "Handler1 click: " << d << "\n";
    });
    ev.on("click", [](const std::string& d) {
        std::cout << "Handler2 click: " << d << "\n";
    });
    ev.on("hover", [](const std::string& d) {
        std::cout << "Hover: " << d << "\n";
    });

    ev.emit("click", "button_ok");
    ev.emit("hover", "menu_item");
    ev.emit("keypress", "X");   // no handlers — silent
}

// ── Function Pointers vs Virtual Functions ────────────────────────────────────
// Virtual functions = compiler-managed function pointer table (vtable)
// Manual function pointers give same dispatch but require explicit management

struct Animal_FP {
    std::string name;
    void (*speak)(const std::string&);  // function pointer for dispatch
};

struct Animal_Virtual {
    std::string name;
    virtual void speak() const = 0;
    virtual ~Animal_Virtual() = default;
};

struct Dog : Animal_Virtual {
    Dog(std::string n) { name = n; }
    void speak() const override { std::cout << name << " says: Woof!\n"; }
};

void bark(const std::string& n) { std::cout << n << " says: Woof!\n"; }
void meow(const std::string& n) { std::cout << n << " says: Meow!\n"; }

void section_vs_virtual() {
    std::cout << "\n=== Function Ptr vs Virtual ===\n";

    // Manual dispatch via function pointer
    Animal_FP animals[] = {
        {"Rex",     bark},
        {"Whiskers",meow}
    };
    for (auto& a : animals) a.speak(a.name);

    // Virtual dispatch
    Dog d("Buddy");
    Animal_Virtual* av = &d;
    av->speak();   // resolved via vtable at runtime
}

int main() {
    std::cout << "╔══ Topic 04: Function Pointers ══╗\n";

    section_basics();
    section_as_argument();
    section_as_return();
    section_array();
    section_member_ptrs();
    section_struct_ptrs();
    section_comparison();
    section_callbacks();
    section_vs_virtual();

    std::cout << "\n✓ Topic 04 complete.\n";
}
