/**
 * TOPIC 06 — Arrays and Strings
 * ==============================
 * Covers: 1D/2D/multi arrays, dynamic arrays, array of pointers,
 *         pointer to array, C strings, std::string, std::string_view,
 *         raw string literals, array decay, std::array, std::span
 *
 * Compile: g++ -std=c++17 -Wall 06_arrays_strings.cpp -o arrays_strings
 */

#include <iostream>
#include <string>
#include <string_view>  // C++17
#include <array>        // std::array
#include <vector>
#include <span>         // C++20 — comment out if using C++17 only
#include <cstring>      // strlen, strcpy, strcmp
#include <algorithm>    // std::sort, std::fill
#include <numeric>      // std::iota

// ── 1D Arrays ────────────────────────────────────────────────────────────────
void section_1d_array() {
    std::cout << "\n=== 1D Arrays ===\n";

    // Stack array — size must be compile-time constant
    int arr[5] = {10, 20, 30, 40, 50};

    // Access by index
    std::cout << "arr[0]=" << arr[0] << " arr[4]=" << arr[4] << "\n";

    // sizeof gives total bytes
    std::cout << "sizeof(arr) = " << sizeof(arr) << " bytes\n";           // 20
    std::cout << "elements    = " << sizeof(arr)/sizeof(arr[0]) << "\n";  // 5

    // Partial initialization — rest zeroed
    int partial[5] = {1, 2};   // {1, 2, 0, 0, 0}
    for (auto x : partial) std::cout << x << " ";
    std::cout << "\n";

    // Zero-initialize all elements
    int zeroed[5]{};            // all zeros
    std::fill(std::begin(zeroed), std::end(zeroed), 7);  // fill with 7
    for (auto x : zeroed) std::cout << x << " ";
    std::cout << "\n";
}

// ── 2D Arrays ────────────────────────────────────────────────────────────────
void section_2d_array() {
    std::cout << "\n=== 2D Arrays ===\n";

    int matrix[3][4] = {
        {1,  2,  3,  4},
        {5,  6,  7,  8},
        {9, 10, 11, 12}
    };

    // Access: matrix[row][col]
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 4; ++c)
            std::cout << matrix[r][c] << "\t";
        std::cout << "\n";
    }

    // Memory layout: row-major — matrix[0][0], [0][1], ..., [2][3] contiguous
    std::cout << "matrix[1][2] via pointer: "
              << *( *( matrix + 1 ) + 2 ) << "\n";   // 7
}

// ── Multidimensional Arrays ───────────────────────────────────────────────────
void section_multi_array() {
    std::cout << "\n=== Multidimensional (3D) ===\n";

    int cube[2][3][4]{};  // 2 layers, 3 rows, 4 cols — zero-initialized

    // Fill with sequential values
    int v = 1;
    for (int l = 0; l < 2; ++l)
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 4; ++c)
                cube[l][r][c] = v++;

    std::cout << "cube[1][2][3] = " << cube[1][2][3] << "\n";   // 24
}

// ── Array Decay ──────────────────────────────────────────────────────────────
// When passed to a function, an array DECAYS to a pointer to its first element.
// The size information is LOST — that's why we pass size separately.
void print_array(int* arr, int n) {
    std::cout << "sizeof inside fn: " << sizeof(arr) << " (ptr size, not array)\n";
    for (int i = 0; i < n; ++i) std::cout << arr[i] << " ";
    std::cout << "\n";
}

// Alternative: pass by reference — preserves type (no decay)
template<int N>
void print_array_ref(int (&arr)[N]) {
    std::cout << "sizeof ref: " << sizeof(arr) << " (full array size)\n";
    for (auto x : arr) std::cout << x << " ";
    std::cout << "\n";
}

void section_decay() {
    std::cout << "\n=== Array Decay ===\n";

    int arr[] = {1, 2, 3, 4, 5};
    std::cout << "sizeof in caller: " << sizeof(arr) << "\n";   // 20

    print_array(arr, 5);       // arr decays to int*
    print_array_ref(arr);      // no decay — template knows size
}

// ── Array of Pointers ─────────────────────────────────────────────────────────
void section_array_of_ptrs() {
    std::cout << "\n=== Array of Pointers ===\n";

    int a = 1, b = 2, c = 3;
    int* ptrs[3] = {&a, &b, &c};

    for (auto* p : ptrs) std::cout << *p << " ";   // 1 2 3
    std::cout << "\n";

    // Array of string pointers (C-style)
    const char* names[] = {"Alice", "Bob", "Carol"};
    for (auto* n : names) std::cout << n << " ";
    std::cout << "\n";
}

// ── Pointer to Array ──────────────────────────────────────────────────────────
void section_ptr_to_array() {
    std::cout << "\n=== Pointer to Array ===\n";

    int arr[5] = {10, 20, 30, 40, 50};

    // Pointer to the WHOLE array — different from pointer to first element
    int (*parr)[5] = &arr;
    //   ─────────── pointer to array of 5 ints

    std::cout << "(*parr)[2] = " << (*parr)[2] << "\n";   // 30

    // Incrementing parr jumps by sizeof(int[5]) = 20 bytes
    // Used for navigating 2D arrays
    int matrix[2][3] = {{1,2,3},{4,5,6}};
    int (*row)[3] = matrix;
    std::cout << "row[1][2] = " << row[1][2] << "\n";     // 6
}

// ── C-style Strings ───────────────────────────────────────────────────────────
void section_cstrings() {
    std::cout << "\n=== C-style Strings ===\n";

    // char array — null-terminated
    char hello[] = "Hello";    // stored as {'H','e','l','l','o','\0'}
    std::cout << "hello = "   << hello          << "\n";
    std::cout << "length = "  << std::strlen(hello) << "\n";   // 5 (not counting \0)
    std::cout << "sizeof = "  << sizeof(hello)  << "\n";       // 6 (includes \0)

    // String literal pointer — read-only memory
    const char* lit = "World";
    // lit[0] = 'w';   // UB — string literals are read-only

    // String manipulation
    char buf[20];
    std::strcpy(buf, hello);    // copy
    std::strcat(buf, " World"); // concatenate
    std::cout << "buf: " << buf << "\n";

    // Comparison
    std::cout << "strcmp: " << std::strcmp("abc", "abc") << "\n";  // 0 (equal)
    std::cout << "strcmp: " << std::strcmp("abc", "abd") << "\n";  // negative

    // Traverse character by character
    for (char* p = hello; *p != '\0'; ++p)
        std::cout << *p << "-";
    std::cout << "\n";
}

// ── std::string ──────────────────────────────────────────────────────────────
void section_std_string() {
    std::cout << "\n=== std::string ===\n";

    std::string s1 = "Hello";
    std::string s2("World");
    std::string s3(5, 'x');   // "xxxxx"

    // Concatenation
    std::string s4 = s1 + ", " + s2 + "!";
    std::cout << s4 << "\n";

    // Size & access
    std::cout << "length: " << s4.length() << "\n";
    std::cout << "s4[0]:  " << s4[0]       << "\n";
    std::cout << "front:  " << s4.front()  << "\n";
    std::cout << "back:   " << s4.back()   << "\n";

    // Searching
    auto pos = s4.find("World");
    std::cout << "find 'World' at: " << pos << "\n";

    // Substring
    std::cout << "substr(7,5): " << s4.substr(7, 5) << "\n";   // "World"

    // Modifying
    s1.append(" C++");
    s1.insert(5, ",");
    s1.replace(0, 5, "Hi");
    std::cout << "modified: " << s1 << "\n";

    // Comparison (lexicographic)
    std::cout << "s1==s2: " << (s1 == s2) << "\n";
    std::cout << "s1< s2: " << (s1 <  s2) << "\n";

    // Conversion
    int    n   = std::stoi("42");
    double d   = std::stod("3.14");
    std::string ns = std::to_string(100);
    std::cout << "stoi: " << n << " stod: " << d << " to_string: " << ns << "\n";

    // Iterating
    for (char c : s2) std::cout << c << "-";
    std::cout << "\n";
}

// ── std::string_view (C++17) ──────────────────────────────────────────────────
// Non-owning, read-only view of a string — zero allocation.
// Works with std::string, const char*, char[], string literals.
void print_sv(std::string_view sv) {
    std::cout << "sv[" << sv.size() << "]: " << sv << "\n";
}

void section_string_view() {
    std::cout << "\n=== std::string_view (C++17) ===\n";

    std::string   s   = "Hello, World!";
    const char*   cs  = "Hello, World!";
    char          arr[] = "Hello, World!";

    print_sv(s);     // from std::string
    print_sv(cs);    // from const char*
    print_sv(arr);   // from char[]
    print_sv("literal");  // from string literal — zero allocation

    // substr without allocation
    std::string_view sv = "Hello, World!";
    std::string_view sub = sv.substr(7, 5);  // view of "World" — no copy!
    std::cout << "substr view: " << sub << "\n";

    // WARNING: string_view does NOT own the data
    // Never return string_view pointing to a local string!
}

// ── String Literals ───────────────────────────────────────────────────────────
void section_literals() {
    std::cout << "\n=== String Literals ===\n";

    // Regular
    const char*  s1 = "Hello\nWorld";   // \n is a newline

    // Raw string literal — no escape processing
    const char*  s2 = R"(Hello\nWorld)";  // \n is literally backslash-n
    std::cout << "regular: " << s1 << "\n";
    std::cout << "raw:      " << s2 << "\n";

    // Raw string with delimiter (useful when content has parentheses)
    const char*  s3 = R"delim(C:\Users\Alice\(files))delim";
    std::cout << "raw path: " << s3 << "\n";

    // Wide, UTF-16, UTF-32
    const wchar_t*   ws  = L"Wide";
    const char16_t*  u16 = u"UTF-16";
    const char32_t*  u32 = U"UTF-32";
    (void)ws; (void)u16; (void)u32;

    // User-defined literal (std::string from literal)
    using namespace std::string_literals;
    std::string str = "Hello"s;    // 's' suffix → std::string
    std::cout << "string literal suffix: " << str << "\n";

    using namespace std::string_view_literals;
    std::string_view sv = "Hello"sv;  // 'sv' suffix → string_view
    std::cout << "string_view literal: " << sv << "\n";
}

// ── std::array (C++11) ───────────────────────────────────────────────────────
// Fixed-size array with STL interface — knows its own size, no decay.
void section_std_array() {
    std::cout << "\n=== std::array ===\n";

    std::array<int, 5> arr = {5, 3, 1, 4, 2};

    std::cout << "size:  "  << arr.size()  << "\n";
    std::cout << "front: "  << arr.front() << "\n";
    std::cout << "back:  "  << arr.back()  << "\n";
    std::cout << "at(2): "  << arr.at(2)   << "\n";   // bounds-checked

    std::sort(arr.begin(), arr.end());
    for (auto x : arr) std::cout << x << " ";
    std::cout << "\n";

    // Unlike raw arrays, std::array can be passed to functions without size
    auto sum = [](const std::array<int,5>& a) {
        int total = 0;
        for (auto x : a) total += x;
        return total;
    };
    std::cout << "sum: " << sum(arr) << "\n";   // 15

    // Comparison works element-wise
    std::array<int,5> b = {1,2,3,4,5};
    std::cout << "arr == b: " << (arr == b) << "\n";   // 1 (true, both sorted)
}

// ── std::span (C++20) ────────────────────────────────────────────────────────
// Non-owning view of contiguous data — works with arrays, vectors, std::array.
// Comment this out if you are on C++17.
void section_span() {
#if __cplusplus >= 202002L
    std::cout << "\n=== std::span (C++20) ===\n";

    int   raw[]   = {1, 2, 3, 4, 5};
    std::vector<int> vec = {10, 20, 30};

    std::span<int> sp1(raw, 5);    // view of raw array
    std::span<int> sp2(vec);       // view of vector

    for (auto x : sp1) std::cout << x << " ";
    std::cout << "\n";
    for (auto x : sp2) std::cout << x << " ";
    std::cout << "\n";

    // Sub-span without copy
    auto sub = sp1.subspan(1, 3);  // elements 1,2,3 (indices 1-3)
    for (auto x : sub) std::cout << x << " ";
    std::cout << "\n";
#else
    std::cout << "\n=== std::span (C++20 — skipped) ===\n";
#endif
}

int main() {
    std::cout << "╔══ Topic 06: Arrays & Strings ══╗\n";

    section_1d_array();
    section_2d_array();
    section_multi_array();
    section_decay();
    section_array_of_ptrs();
    section_ptr_to_array();
    section_cstrings();
    section_std_string();
    section_string_view();
    section_literals();
    section_std_array();
    section_span();

    std::cout << "\n✓ Topic 06 complete.\n";
}
