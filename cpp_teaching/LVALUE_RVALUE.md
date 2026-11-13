# lvalue & rvalue in C++ — Complete Guide

## Table of Contents
1. [The Core Idea](#the-core-idea)
2. [lvalue](#lvalue)
3. [rvalue](#rvalue)
4. [The Full Value Category Tree](#the-full-value-category-tree)
5. [References: lvalue ref vs rvalue ref](#references-lvalue-ref-vs-rvalue-ref)
6. [std::move](#stdmove)
7. [Move Constructor & Move Assignment](#move-constructor--move-assignment)
8. [Perfect Forwarding & std::forward](#perfect-forwarding--stdforward)
9. [Return Value Optimization (RVO)](#return-value-optimization-rvo)
10. [Practical Rules of Thumb](#practical-rules-of-thumb)
11. [Common Mistakes](#common-mistakes)
12. [Quick Reference Cheat Sheet](#quick-reference-cheat-sheet)

---

## The Core Idea

Every expression in C++ has two properties:
- **a type** — `int`, `std::string`, `double*`, etc.
- **a value category** — where it lives and whether it can be moved from

The simplest mental model:

| Question | Category |
|---|---|
| Does it have a name? Can I take its address? | **lvalue** |
| Is it a temporary? Will it die at the end of this expression? | **rvalue** |

```cpp
int x = 5;
//  ↑       ↑
// lvalue  rvalue (temporary literal, dies immediately)

std::string s = std::string("hello");
//          ↑   ───────────────────
//        lvalue      rvalue (temporary object)
```

---

## lvalue

> **"l"** originally stood for **left** side of `=`, but the real meaning is:
> *a named, addressable, persistent object.*

### Characteristics
- Has a **name** (variable, parameter, member)
- Has a **stable memory address** — `&x` works
- **Persists** beyond the current expression
- Can appear on **either side** of `=` (if not const)

### Examples

```cpp
int a = 10;          // 'a' is lvalue
a = 20;              // lvalue on left side — OK

int* p = &a;         // &a — taking address works because 'a' is lvalue

std::string s = "hi";
s = "world";         // 's' is lvalue

// Function that returns lvalue reference — the call is an lvalue
int& get_ref(int& x) { return x; }
get_ref(a) = 99;     // OK! — the return is an lvalue, can assign to it

// Array elements are lvalues
int arr[3] = {1, 2, 3};
arr[0] = 10;         // arr[0] is lvalue

// Struct/class members are lvalues (when the object is an lvalue)
struct Point { int x, y; };
Point pt{1, 2};
pt.x = 5;            // pt.x is lvalue
```

### What is NOT an lvalue

```cpp
42          // literal — no name, no address
a + b       // result of expression — temporary
std::string("hello")  // temporary object
```

---

## rvalue

> **"r"** originally stood for **right** side of `=`, but the real meaning is:
> *a temporary value with no persistent identity.*

### Characteristics
- **No name** (or its name is about to expire)
- **No stable address** — `&(a + b)` is illegal
- **Temporary** — dies at the end of the full expression
- Can only appear on the **right side** of `=`
- **Can be moved from** — its resources can be stolen

### Two kinds of rvalue

#### 1. prvalue ("pure rvalue") — a computed temporary
```cpp
42              // integer literal
3.14            // float literal
true            // bool literal
a + b           // arithmetic result
std::string("hi")  // temporary object constructed inline
func_returning_by_value()  // function that returns T (not T&)
```

#### 2. xvalue ("expiring value") — a named object you've marked as "done with"
```cpp
std::move(s)    // you explicitly said "I'm done with s, steal it"
std::move(v)    // moves vector v
```

---

## The Full Value Category Tree

```
Expression
├── glvalue  ("generalized lvalue" — has identity/address)
│   ├── lvalue    — has identity, cannot be moved from
│   └── xvalue    — has identity, CAN be moved from  (std::move result)
│
└── rvalue   (can be moved from)
    ├── xvalue    — same as above (the overlap)
    └── prvalue   — pure temporary, no identity
```

In practice you only need to think about three:

```
lvalue   — named, addressable, persistent
xvalue   — named but "expiring" (std::move)
prvalue  — pure temporary (literal, expression result)
```

---

## References: lvalue ref vs rvalue ref

### lvalue reference  `T&`

Binds to **lvalues only** (named, persistent objects).

```cpp
int a = 10;

int& ref = a;       // OK — 'a' is lvalue
ref = 20;           // modifies 'a'

// int& bad = 42;   // ERROR — 42 is rvalue, can't bind to lvalue ref
// int& bad = a+1;  // ERROR — a+1 is rvalue
```

**const lvalue reference** `const T&` — special rule: binds to BOTH lvalues and rvalues.
This is why functions taking `const T&` accept everything.

```cpp
const int& cr1 = a;    // OK — lvalue
const int& cr2 = 42;   // OK — rvalue (lifetime extended to match cr2)
const int& cr3 = a+1;  // OK — rvalue
```

### rvalue reference  `T&&`

Binds to **rvalues only** (temporaries and `std::move`d objects).

```cpp
int&& rr1 = 42;         // OK — 42 is rvalue
int&& rr2 = a + 1;      // OK — a+1 is rvalue

// int&& bad = a;        // ERROR — 'a' is lvalue
int&& rr3 = std::move(a);  // OK — std::move turns a into xvalue
```

### Side-by-side comparison

```cpp
void foo(int& x)        { std::cout << "lvalue ref\n"; }
void foo(int&& x)       { std::cout << "rvalue ref\n"; }
void foo(const int& x)  { std::cout << "const lvalue ref\n"; }

int a = 5;

foo(a);              // calls foo(int&)        — a is lvalue
foo(42);             // calls foo(int&&)       — 42 is rvalue
foo(a + 1);          // calls foo(int&&)       — a+1 is rvalue
foo(std::move(a));   // calls foo(int&&)       — std::move makes it rvalue
```

---

## std::move

`std::move` does **not move anything**. It is purely a **cast** to rvalue reference.

```cpp
// What std::move actually is (simplified):
template<typename T>
std::remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}
```

It just says: *"treat this named object as a temporary — it's safe to steal from it."*

The **actual moving** happens in the **move constructor** or **move assignment operator**.

```cpp
std::string a = "Hello World";   // 'a' owns heap memory

// WITHOUT move:
std::string b = a;               // COPY — allocates new heap memory, copies chars
                                 // 'a' unchanged, both have their own copy

// WITH move:
std::string c = std::move(a);    // MOVE — 'c' steals 'a's internal pointer
                                 // 'a' is now empty ("")
                                 // zero heap allocation
```

### When is `std::move` worth using?

| Type | Copy cost | Move cost | Use move? |
|---|---|---|---|
| `int`, `double`, `char` | O(1) | O(1) | No benefit |
| `std::string` | O(n) chars | O(1) pointer swap | ✓ Yes |
| `std::vector<T>` | O(n) elements | O(1) pointer swap | ✓ Yes |
| `std::unique_ptr` | ❌ deleted | O(1) pointer swap | ✓ Yes (required) |
| `std::array<T,N>` | O(n) | O(n) — no heap to steal | No benefit |

```cpp
// Practical patterns

// 1. Sink parameter — function that takes ownership
void store(std::string s) { ... }   // 's' is already a copy/move
std::string name = "Alice";
store(std::move(name));             // move instead of copy — name is now ""

// 2. Moving into a container
std::vector<std::string> v;
std::string s = "expensive string";
v.push_back(std::move(s));         // s's data moved into vector element

// 3. Moving a unique_ptr (required — can't copy)
auto p = std::make_unique<int>(42);
auto q = std::move(p);             // p is now null, q owns the int

// 4. Moving a member in a constructor initializer list
struct Wrapper {
    std::string data;
    explicit Wrapper(std::string d) : data(std::move(d)) {}
    //                                     ──────────────
    //                                     'd' is a local parameter (lvalue)
    //                                     move it into 'data' — avoids a copy
};
```

---

## Move Constructor & Move Assignment

A class gets these automatically if you follow the Rule of Zero.
But understanding them helps you grasp why `std::move` is fast.

```cpp
class Buffer {
    char* data_;
    size_t size_;

public:
    // Constructor
    explicit Buffer(size_t sz) : data_(new char[sz]), size_(sz) {
        std::cout << "construct\n";
    }

    // Destructor
    ~Buffer() { delete[] data_; std::cout << "destroy\n"; }

    // ── Copy constructor — deep copy, O(n) ──────────────────────────────────
    Buffer(const Buffer& other) : data_(new char[other.size_]), size_(other.size_) {
        std::memcpy(data_, other.data_, size_);
        std::cout << "copy\n";
    }

    // ── Move constructor — pointer steal, O(1) ──────────────────────────────
    Buffer(Buffer&& other) noexcept
        : data_(other.data_),    // steal the pointer
          size_(other.size_)
    {
        other.data_ = nullptr;   // leave source in valid empty state
        other.size_ = 0;
        std::cout << "move\n";
    }

    // ── Copy assignment ──────────────────────────────────────────────────────
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data_;
            data_ = new char[other.size_];
            size_ = other.size_;
            std::memcpy(data_, other.data_, size_);
        }
        std::cout << "copy assign\n";
        return *this;
    }

    // ── Move assignment ──────────────────────────────────────────────────────
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;          // free our old resource
            data_ = other.data_;     // steal
            size_ = other.size_;
            other.data_ = nullptr;   // leave source valid
            other.size_ = 0;
        }
        std::cout << "move assign\n";
        return *this;
    }
};

void demo_move_semantics() {
    Buffer a(100);                // construct
    Buffer b = a;                 // copy    — allocates 100 bytes
    Buffer c = std::move(a);      // move    — just swaps pointers, O(1)
    b = std::move(c);             // move assign
}
```

### The Rule of Five / Zero

- **Rule of Zero**: If your class only holds smart pointers / STL containers, don't write any of the five. The compiler generates correct ones automatically.
- **Rule of Five**: If you write ANY of {destructor, copy ctor, copy assign, move ctor, move assign}, you should explicitly define ALL five.

```cpp
// Rule of Zero — preferred
struct Person {
    std::string name;    // string handles its own memory
    int age;
    // No destructor, no copy/move — compiler generates correct ones
};

// Rule of Five — only needed for raw resource management
class FileHandle {
    FILE* fp_;
public:
    explicit FileHandle(const char* path) : fp_(fopen(path, "r")) {}
    ~FileHandle()                              { if (fp_) fclose(fp_); }
    FileHandle(const FileHandle&)            = delete;  // can't copy a file handle
    FileHandle& operator=(const FileHandle&) = delete;
    FileHandle(FileHandle&& o) noexcept      : fp_(o.fp_) { o.fp_ = nullptr; }
    FileHandle& operator=(FileHandle&& o) noexcept {
        if (this != &o) { if (fp_) fclose(fp_); fp_ = o.fp_; o.fp_ = nullptr; }
        return *this;
    }
};
```

---

## Perfect Forwarding & std::forward

**Problem**: when you receive a parameter in a template function and want to pass it on, you lose its value category.

```cpp
template<typename T>
void wrapper(T arg) {
    target(arg);    // 'arg' is ALWAYS an lvalue here (it has a name!)
                    // even if the caller passed an rvalue
}

wrapper(42);           // 42 is rvalue, but inside wrapper, arg is lvalue → target gets lvalue
wrapper(std::move(s)); // move, but target still gets lvalue!
```

**Solution**: universal reference `T&&` + `std::forward<T>`

```cpp
template<typename T>
void wrapper(T&& arg) {
    //         ──── universal reference (not rvalue ref!)
    //              deduces as lvalue ref or rvalue ref based on what was passed

    target(std::forward<T>(arg));
    //     ───────────────────── preserves the original value category
}

int a = 5;
wrapper(a);            // T deduced as int&  → forwards as lvalue
wrapper(42);           // T deduced as int   → forwards as rvalue
wrapper(std::move(a)); // T deduced as int   → forwards as rvalue
```

### Real-world usage: `make_unique` / `emplace_back`

```cpp
// emplace_back uses perfect forwarding to construct in-place
std::vector<std::string> v;
v.emplace_back("hello");          // constructs string("hello") directly in vector
                                  // no temporary string created

// make_unique uses perfect forwarding
auto p = std::make_unique<std::string>("world");
// equivalent to: new std::string("world")
// but exception-safe
```

---

## Return Value Optimization (RVO)

The compiler often **elides the copy/move entirely** when returning a local object.

```cpp
std::string make_greeting(const std::string& name) {
    std::string result = "Hello, " + name + "!";
    return result;    // RVO — result is constructed DIRECTLY in the caller's location
                      // no copy, no move — zero overhead
}

auto g = make_greeting("Alice");
// 'result' inside the function and 'g' in the caller ARE THE SAME OBJECT
```

**Named RVO (NRVO)** — same idea for named local variables.
**Mandatory copy elision (C++17)** — for prvalues, elision is GUARANTEED:

```cpp
std::string get() {
    return std::string("hello");  // C++17: guaranteed no copy or move
}
auto s = get();   // constructed directly — zero copies guaranteed by standard
```

> **Rule**: Don't `std::move` a return value — it can actually *prevent* RVO!
> ```cpp
> // BAD — prevents RVO
> return std::move(result);
>
> // GOOD — allows RVO (or move if RVO can't apply)
> return result;
> ```

---

## Practical Rules of Thumb

### 1. Function parameter passing

```cpp
// Small/trivial types (int, double, char, pointer) — pass by value
void foo(int x);

// Read-only, don't know the size — pass by const lvalue ref
void foo(const std::string& s);
void foo(const std::vector<T>& v);

// Read-only string-like — pass by string_view (C++17, zero allocation)
void foo(std::string_view sv);

// Taking ownership — pass by value (lets caller choose copy or move)
void store(std::string s);    // caller: store(name)           → copy
                              //         store(std::move(name)) → move

// Modifying caller's object — pass by lvalue ref
void append(std::string& s);

// Forwarding in templates — universal reference
template<typename T> void forward_to(T&& arg);
```

### 2. When to use `std::move`

```cpp
// ✓ Moving into a container
vec.push_back(std::move(heavy_object));

// ✓ Moving in constructor initializer list
Foo(std::string s) : member_(std::move(s)) {}

// ✓ Moving a unique_ptr (copy is deleted)
auto q = std::move(unique_ptr_p);

// ✓ Moving return value from a local that won't benefit from RVO
// (rare — usually just return the name)

// ✗ DON'T move a return value (prevents RVO)
return std::move(local_var);   // BAD

// ✗ DON'T use after move — object is in valid-but-unspecified state
auto q = std::move(p);
p->foo();   // BAD — p might be null/empty

// ✗ DON'T move small/trivial types — no benefit, hurts readability
std::move(42);        // pointless
std::move(some_int);  // pointless
```

---

## Common Mistakes

### Mistake 1: Using an object after `std::move`

```cpp
std::string s = "data";
auto t = std::move(s);
std::cout << s;    // BUG — s is in unspecified state (probably "")
                   // May work, may not — undefined behavior in general
```

**Fix**: treat a moved-from object as if it doesn't exist. Reassign before reuse.

```cpp
auto t = std::move(s);
s = "new value";   // safe to use again after reassignment
```

---

### Mistake 2: Returning `std::move` (prevents RVO)

```cpp
// BAD
std::string make() {
    std::string result = "hello";
    return std::move(result);   // forces a move instead of elision
}

// GOOD
std::string make() {
    std::string result = "hello";
    return result;              // compiler elides or moves automatically
}
```

---

### Mistake 3: `const` rvalue reference (useless)

```cpp
// You almost never want this:
void foo(const std::string&& s);
// Can't move from it (const), can't modify it.
// Use const std::string& instead.
```

---

### Mistake 4: Thinking `T&&` in a template is always rvalue ref

```cpp
template<typename T>
void foo(T&& arg) { ... }
// This is a UNIVERSAL REFERENCE (forwarding reference)
// NOT necessarily an rvalue reference!
// It deduces to lvalue ref when passed an lvalue.

void bar(int&& x) { ... }
// This IS an rvalue reference — no template deduction happening
```

---

### Mistake 5: Moving a `const` object (silently copies)

```cpp
const std::string s = "hello";
auto t = std::move(s);    // Calls COPY constructor, not move!
                          // Can't move from const — no warning, silent copy
```

---

## Quick Reference Cheat Sheet

```
CATEGORIES
──────────────────────────────────────────────────────
lvalue    Has name, has address, persists              → int x, arr[i], *p
xvalue    Has name but expiring (std::move result)     → std::move(x)
prvalue   Temporary, no name                           → 42, x+y, func()

REFERENCES
──────────────────────────────────────────────────────
T&         lvalue ref    — binds to lvalues only
const T&   const lref    — binds to BOTH lvalues and rvalues
T&&        rvalue ref    — binds to rvalues (temporaries, moved objects)
T&&        universal ref — in template context: deduces to either

FUNCTIONS
──────────────────────────────────────────────────────
std::move(x)         — cast x to rvalue ref (does NOT move)
std::forward<T>(x)   — preserve value category in templates

PARAMETER PASSING GUIDE
──────────────────────────────────────────────────────
int, double, char*          → by value
read-only, any size         → const T&  or  string_view
taking ownership            → by value (caller picks copy/move)
modifying caller's object   → T&
templates                   → T&&  +  std::forward<T>

PERFORMANCE QUICK WINS
──────────────────────────────────────────────────────
push_back(std::move(obj))   — move instead of copy into container
emplace_back(args...)       — construct in-place, no temporary at all
: member_(std::move(arg))   — move in initializer list
return local_var;           — let RVO do its job, no std::move needed
```
