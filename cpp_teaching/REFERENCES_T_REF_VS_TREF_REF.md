# `T&` vs `T&&` in C++ — Complete Guide

## Table of Contents
1. [Quick Answer](#quick-answer)
2. [T& — lvalue reference](#t--lvalue-reference)
3. [T&& — rvalue reference](#t--rvalue-reference)
4. [const T& — the universal read-only reference](#const-t--the-universal-read-only-reference)
5. [T&& in a template — Universal Reference](#t-in-a-template--universal-reference)
6. [Reference Collapsing Rules](#reference-collapsing-rules)
7. [std::forward — the right tool for universal references](#stdforward--the-right-tool-for-universal-references)
8. [Function Overload Resolution with References](#function-overload-resolution-with-references)
9. [Real Usage Cases Side by Side](#real-usage-cases-side-by-side)
10. [Move Constructor & Move Assignment using T&&](#move-constructor--move-assignment-using-t)
11. [Common Mistakes](#common-mistakes)
12. [Cheat Sheet](#cheat-sheet)

---

## Quick Answer

```
T&   — lvalue reference   — binds to named, persistent objects
T&&  — rvalue reference   — binds to temporaries or std::move'd objects
```

But there is a catch:

```
T&&  in a TEMPLATE       — universal reference — binds to ANYTHING
T&&  in a NON-template   — rvalue reference    — binds to rvalues only
```

This distinction is the source of most confusion. The guide covers both.

---

## T& — lvalue reference

An lvalue reference is an **alias** for an existing named object.
It must bind to something that **has a name and an address**.

```cpp
int a = 10;

int& ref = a;    // ref is an alias for 'a'
ref = 99;        // modifies 'a' — they are the same object
std::cout << a;  // 99

// What it CANNOT bind to:
// int& r1 = 42;        // ERROR — 42 is a temporary (rvalue)
// int& r2 = a + 1;     // ERROR — a+1 is a temporary (rvalue)
// int& r3 = func();    // ERROR — if func() returns by value (rvalue)
```

### Properties of `T&`

| Property | Value |
|---|---|
| Binds to lvalues | ✓ Yes |
| Binds to rvalues | ✗ No |
| Binds to temporaries | ✗ No |
| Can modify the bound object | ✓ Yes |
| Must be initialized | ✓ Yes |
| Can be reseated (changed to alias something else) | ✗ No |

### When a function takes `T&`

```cpp
void increment(int& x) {
    x++;    // modifies the caller's variable directly
}

int n = 5;
increment(n);       // OK — n is lvalue
std::cout << n;     // 6

// increment(42);   // ERROR — 42 is rvalue, can't bind to int&
// increment(n+1);  // ERROR — n+1 is rvalue
```

---

## T&& — rvalue reference

An rvalue reference binds to **temporaries** and **`std::move`d objects**.
Its purpose is to enable **move semantics** — stealing resources from objects
that are about to be destroyed anyway.

```cpp
int&&  rr1 = 42;         // OK — 42 is a temporary rvalue
int&&  rr2 = 3 + 4;      // OK — 3+4 is a temporary rvalue
// int&& rr3 = n;         // ERROR — n is lvalue, not an rvalue

int n = 10;
int&& rr4 = std::move(n);  // OK — std::move casts n to rvalue
// Now rr4 refers to n, and you've signalled "n can be moved from"
```

### Properties of `T&&`

| Property | Value |
|---|---|
| Binds to lvalues | ✗ No |
| Binds to rvalues | ✓ Yes |
| Binds to temporaries | ✓ Yes |
| Binds to `std::move(x)` | ✓ Yes |
| Can modify the bound object | ✓ Yes |
| Signals "I may steal this" | ✓ Yes |

### Key insight: once named, an rvalue reference IS an lvalue

```cpp
void foo(std::string&& s) {
    // Inside here, 's' has a name → 's' is an LVALUE
    // Even though it was passed as an rvalue

    std::string local = s;           // COPY — s is lvalue inside here!
    std::string moved = std::move(s); // MOVE — you must move explicitly
}
```

This is the most common source of bugs with `T&&`. See [Common Mistakes](#common-mistakes).

---

## `const T&` — the universal read-only reference

`const T&` is special — it binds to **everything**: lvalues, rvalues, temporaries.
And it **extends the lifetime** of temporaries to match the reference.

```cpp
int n = 5;

const int& r1 = n;       // OK — lvalue
const int& r2 = 42;      // OK — rvalue literal
const int& r3 = n + 1;   // OK — rvalue expression
const int& r4 = func();  // OK — rvalue (function return by value)

// r2 = 99;              // ERROR — const, can't modify
```

### Why does this exist?

Before C++11, there were no rvalue references. `const T&` was the only way to
write a function that accepted both named objects and temporaries:

```cpp
// Accepts everything — the classic pre-C++11 "accept-all" parameter
void log(const std::string& msg) {
    std::cout << msg;
}

std::string s = "hello";
log(s);           // lvalue — OK
log("world");     // rvalue string literal — OK (implicit conversion to string)
log(s + "!");     // rvalue temporary — OK
```

### `const T&` vs `T&&` — the key difference

```cpp
void take_const_ref(const std::string& s) {
    // Can READ s, cannot move from it (it's const)
    std::string copy = s;        // must copy — can't move a const
}

void take_rvalue_ref(std::string&& s) {
    // Can READ and MODIFY s, and should MOVE from it
    std::string moved = std::move(s);  // efficient steal
}
```

---

## T&& in a template — Universal Reference

This is where `T&&` changes meaning entirely.

When `T&&` appears in a **function template** where `T` is a **deduced type**,
it is called a **universal reference** (or **forwarding reference**).
It binds to **both lvalues and rvalues**.

```cpp
// NON-TEMPLATE — T&& is a plain rvalue reference
void foo(std::string&& s);    // only accepts rvalues

// TEMPLATE — T&& is a UNIVERSAL reference
template<typename T>
void bar(T&& arg);            // accepts ANYTHING — lvalue or rvalue
```

```cpp
template<typename T>
void bar(T&& arg) {
    // arg can be lvalue ref or rvalue ref depending on what was passed
}

int n = 5;
bar(n);             // T deduced as int&  → arg is int&  (lvalue ref)
bar(42);            // T deduced as int   → arg is int&& (rvalue ref)
bar(std::move(n));  // T deduced as int   → arg is int&& (rvalue ref)
```

### Three conditions for a universal reference

ALL three must be true:

1. The type must be **`T&&`** — not `const T&&`, not `std::vector<T>&&`
2. `T` must be **deduced** (from the function argument)
3. There must be **no other qualifiers** on `T`

```cpp
template<typename T>
void f1(T&& x);            // ✓ universal reference

template<typename T>
void f2(const T&& x);      // ✗ NOT universal — const present, only rvalue ref

template<typename T>
void f3(std::vector<T>&& x); // ✗ NOT universal — T is inside vector<>, only rvalue ref

template<typename T>
class MyClass {
    void method(T&& x);    // ✗ NOT universal — T is fixed by class, not deduced here
};

auto lambda = [](auto&& x) { };  // ✓ universal reference — 'auto&&' is same rule
```

---

## Reference Collapsing Rules

When you have a reference to a reference (which C++ allows only through templates),
the compiler **collapses** them using these rules:

```
T&  &   →  T&
T&  &&  →  T&
T&& &   →  T&
T&& &&  →  T&&
```

**Simple rule**: if ANY of them is `&`, the result is `&`.
Only `&& + &&` stays `&&`.

This is what makes universal references work:

```cpp
template<typename T>
void bar(T&& arg);

int n = 5;

// Calling with lvalue:
bar(n);
// T is deduced as int& (because n is lvalue)
// arg type becomes: int& && → collapses to int&
// So arg is int& — lvalue reference ✓

// Calling with rvalue:
bar(42);
// T is deduced as int (no reference — rvalue)
// arg type becomes: int&& — no collapsing needed
// So arg is int&& — rvalue reference ✓
```

---

## std::forward — the right tool for universal references

Inside a template function, a parameter named `arg` is **always an lvalue** (it has a name).
`std::forward<T>(arg)` restores the **original value category** of what was passed in.

```cpp
// WITHOUT std::forward — always passes as lvalue (wrong!)
template<typename T>
void broken_wrapper(T&& arg) {
    target(arg);              // arg has a name → always lvalue!
                              // even if the caller passed an rvalue
}

// WITH std::forward — preserves original value category (correct!)
template<typename T>
void correct_wrapper(T&& arg) {
    target(std::forward<T>(arg));
    //     ───────────────────── if caller passed lvalue  → forwards as lvalue
    //                           if caller passed rvalue  → forwards as rvalue
}
```

### How `std::forward` works

```cpp
// Simplified implementation
template<typename T>
T&& forward(std::remove_reference_t<T>& arg) noexcept {
    return static_cast<T&&>(arg);
}
```

When `T = int&`:  `forward` returns `int& &&` → collapses to `int&`  (lvalue)
When `T = int`:   `forward` returns `int&&`                           (rvalue)

### std::move vs std::forward

```cpp
std::move(x)         // ALWAYS produces rvalue — unconditional cast
std::forward<T>(x)   // CONDITIONALLY produces rvalue — only if T is non-reference
```

```cpp
// std::move — use when you OWN the object and want to give it up
std::string s = "hello";
some_func(std::move(s));         // s is gone, you gave it away

// std::forward — use in templates to PASS THROUGH the value category
template<typename T>
void relay(T&& arg) {
    some_func(std::forward<T>(arg));   // preserves whatever was passed in
}
```

---

## Function Overload Resolution with References

When you have multiple overloads, the compiler picks the best match:

```cpp
void process(int& x)        { std::cout << "lvalue ref\n";       }
void process(const int& x)  { std::cout << "const lvalue ref\n"; }
void process(int&& x)       { std::cout << "rvalue ref\n";       }

int n = 5;
const int cn = 10;

process(n);              // → lvalue ref       (best match: int&)
process(cn);             // → const lvalue ref (best match: const int&)
process(42);             // → rvalue ref       (best match: int&&)
process(std::move(n));   // → rvalue ref       (best match: int&&)
```

### Priority when overloads exist

```
For lvalue:       int&  >  const int&  >  (no match for int&&)
For const lvalue: const int&            >  (no match for int& or int&&)
For rvalue:       int&& >  const int&  >  (no match for int&)
```

If only `const T&` exists, it matches everything (fallback):
```cpp
void log(const int& x) { std::cout << x; }

log(n);              // OK — const int& accepts lvalue
log(42);             // OK — const int& accepts rvalue
log(std::move(n));   // OK — const int& accepts rvalue
```

---

## Real Usage Cases Side by Side

### Case 1: Modifying the caller's object → use `T&`

```cpp
// T& — in/out parameter, caller's variable is modified
void append_suffix(std::string& s, const std::string& suffix) {
    s += suffix;
}

std::string name = "Alice";
append_suffix(name, "_admin");
std::cout << name;   // "Alice_admin"

// append_suffix("Bob", "_admin");  // ERROR — literal is rvalue
```

---

### Case 2: Read-only, any input → use `const T&`

```cpp
// const T& — read-only, accepts everything, no copy
void print(const std::string& s) {
    std::cout << s << "\n";
}

std::string s = "hello";
print(s);           // lvalue — OK
print("world");     // rvalue — OK
print(s + "!");     // temporary — OK
```

---

### Case 3: Stealing ownership → use `T&&`

```cpp
// T&& — take ownership, move the resource in
class Queue {
    std::vector<std::string> items_;
public:
    void push(std::string&& item) {
        items_.push_back(std::move(item));  // steal item's buffer
    }
};

Queue q;
std::string msg = "important message";
q.push(std::move(msg));    // msg is now empty, Queue owns the string
// q.push(msg);            // ERROR — msg is lvalue, can't bind to string&&
q.push("temporary");       // OK — string literal creates rvalue temporary
```

---

### Case 4: Accept both lvalue and rvalue, forward efficiently → use `T&&` + `std::forward`

```cpp
// Universal reference + forward — the "perfect forwarding" pattern
// Used in factory functions, wrappers, emplace-style functions

template<typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
// This is essentially what std::make_unique does internally

struct Person {
    std::string name;
    int age;
    Person(std::string n, int a) : name(std::move(n)), age(a) {}
};

std::string name = "Alice";
auto p1 = make<Person>(name, 30);          // name copied (lvalue forwarded as lvalue)
auto p2 = make<Person>(std::move(name), 25); // name moved (rvalue forwarded as rvalue)
auto p3 = make<Person>("Bob", 40);           // temporary moved (rvalue forwarded as rvalue)
```

---

### Case 5: Sink parameter — caller decides copy or move → pass `T` by value

```cpp
// By value — caller chooses:  copy if they want to keep it, move if they don't
class Logger {
    std::string prefix_;
public:
    explicit Logger(std::string prefix)      // by value — not T& or T&&
        : prefix_(std::move(prefix)) {}      // move into member
};

std::string pfx = "[INFO]";
Logger L1(pfx);               // copy (pfx still usable)
Logger L2(std::move(pfx));    // move (pfx is now empty — cheap)
Logger L3("[DEBUG]");         // from literal — one construction, no copy
```

---

### Case 6: `auto&&` — generic lambdas and range-for

```cpp
// auto&& in a lambda — universal reference, works for any type
auto print = [](auto&& val) {
    std::cout << std::forward<decltype(val)>(val) << "\n";
};

print(42);                // rvalue
print(std::string("hi")); // rvalue
int n = 5;
print(n);                 // lvalue

// auto&& in range-for — most efficient: no copy, works for proxy types
std::vector<bool> flags = {true, false, true};
for (auto&& flag : flags) {    // auto&& handles vector<bool>'s proxy correctly
    flag = !flag;              // modifies in-place
}
// for (auto flag : flags)  — would copy; for vector<bool> this is a proxy object
// for (auto& flag : flags) — may not compile for proxy types
```

---

## Move Constructor & Move Assignment using T&&

The classic use of `T&&` in a class:

```cpp
class Buffer {
    int*   data_;
    size_t size_;

public:
    // Constructor
    explicit Buffer(size_t n)
        : data_(new int[n]), size_(n) {}

    // Destructor
    ~Buffer() { delete[] data_; }

    // ── Copy constructor — T& (const) ───────────────────────────────────────
    Buffer(const Buffer& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
        // Deep copy — allocates new memory, copies every element
    }

    // ── Move constructor — T&& ───────────────────────────────────────────────
    Buffer(Buffer&& other) noexcept
        : data_(other.data_),    // steal the pointer
          size_(other.size_) {
        other.data_ = nullptr;   // leave source in valid empty state
        other.size_ = 0;
        // O(1) — no allocation, no copy, just pointer assignment
    }

    // ── Copy assignment — const T& ───────────────────────────────────────────
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data_;
            data_ = new int[other.size_];
            size_ = other.size_;
            std::copy(other.data_, other.data_ + size_, data_);
        }
        return *this;
    }

    // ── Move assignment — T&& ────────────────────────────────────────────────
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;          // free OUR old memory
            data_ = other.data_;     // steal theirs
            size_ = other.size_;
            other.data_ = nullptr;   // leave source valid
            other.size_ = 0;
        }
        return *this;
    }
};

Buffer a(100);
Buffer b = a;             // copy constructor  — 100 int allocations
Buffer c = std::move(a);  // move constructor  — just pointer copy, O(1)
c = std::move(b);         // move assignment   — just pointer swap, O(1)
```

---

## Common Mistakes

### Mistake 1: Forgetting to `std::move` inside a `T&&` function

```cpp
// WRONG — always copies, never moves
void store(std::string&& s) {
    data_ = s;              // s has a name → lvalue → COPY
}

// CORRECT — explicitly move
void store(std::string&& s) {
    data_ = std::move(s);   // cast back to rvalue → MOVE
}
```

---

### Mistake 2: Using `std::move` instead of `std::forward` in templates

```cpp
// WRONG — always moves, even when caller passed an lvalue
template<typename T>
void broken(T&& arg) {
    target(std::move(arg));    // moves even if arg was lvalue → caller's object destroyed!
}

// CORRECT — preserves original category
template<typename T>
void correct(T&& arg) {
    target(std::forward<T>(arg));
}
```

---

### Mistake 3: Thinking `T&&` in a template is always rvalue ref

```cpp
template<typename T>
void foo(T&& x) { }   // universal reference — NOT "rvalue ref only"

foo(42);              // rvalue — yes
int n = 5;
foo(n);               // lvalue — also works! T deduced as int&
```

---

### Mistake 4: Returning an rvalue reference to a local

```cpp
// WRONG — dangling reference! local is destroyed when function returns
std::string&& bad() {
    std::string local = "hello";
    return std::move(local);    // UB — refers to destroyed object
}

// CORRECT — return by value, let RVO do its job
std::string good() {
    std::string local = "hello";
    return local;               // RVO or move, no dangling
}
```

---

### Mistake 5: Overloading both `T&` and `T&&` when `const T&` is enough

```cpp
// Unnecessary — three overloads doing similar things
void log(std::string& s)        { std::cout << s; }
void log(const std::string& s)  { std::cout << s; }
void log(std::string&& s)       { std::cout << s; }

// Simpler — const T& accepts everything for read-only
void log(const std::string& s)  { std::cout << s; }
// or for string specifically, string_view is even better:
void log(std::string_view s)    { std::cout << s; }
```

---

## Cheat Sheet

```
BINDING RULES
─────────────────────────────────────────────────────────────────
Reference Type   | lvalue | const lvalue | rvalue | std::move(x)
─────────────────|--------|--------------|--------|─────────────
T&               |  ✓     |      ✗       |   ✗    |     ✗
const T&         |  ✓     |      ✓       |   ✓    |     ✓
T&&              |  ✗     |      ✗       |   ✓    |     ✓
T&& (template)   |  ✓     |      ✓       |   ✓    |     ✓
─────────────────────────────────────────────────────────────────

CHOOSING THE RIGHT PARAMETER TYPE
─────────────────────────────────────────────────────────────────
Goal                               → Use
─────────────────────────────────────────────────────────────────
Modify caller's object             → T&
Read-only, accept anything         → const T&
Steal ownership (move-only)        → T&&  with std::move inside
Accept & forward perfectly         → T&&  with std::forward<T>
Caller decides copy or move        → T    by value + std::move into member
Read-only strings, zero alloc      → std::string_view

INSIDE A T&& FUNCTION
─────────────────────────────────────────────────────────────────
Non-template T&&   → std::move(arg)     to actually move
Template   T&&     → std::forward<T>(arg) to preserve category

std::move(x)       → ALWAYS rvalue      (unconditional)
std::forward<T>(x) → CONDITIONAL rvalue (only if T was rvalue)

REFERENCE COLLAPSING  (template mechanics)
─────────────────────────────────────────────────────────────────
T&  &   →  T&     any & wins
T&  &&  →  T&     any & wins
T&& &   →  T&     any & wins
T&& &&  →  T&&    only && stays when both are &&

UNIVERSAL REFERENCE — when T&& is NOT just rvalue ref
─────────────────────────────────────────────────────────────────
template<typename T> void f(T&& x)   ✓ universal ref
                     void f(Foo&& x) ✗ rvalue ref only
auto&& x = expr;                     ✓ universal ref
```
