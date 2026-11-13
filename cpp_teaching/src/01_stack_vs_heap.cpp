/**
 * LESSON 01 — Stack vs Heap Memory
 * =================================
 * Key ideas:
 *  - Stack: automatic storage, LIFO, fast, limited size (~1-8 MB).
 *  - Heap: dynamic storage, manual lifetime, large, slightly slower.
 *  - new / delete vs stack-local variables.
 *  - Stack overflow demo (commented out — dangerous!).
 */

#include <iostream>
#include <array>
#include <chrono>
#include <vector>

// ── 1. Stack allocation ──────────────────────────────────────────────────────
void stack_demo() {
    std::cout << "\n=== Stack Allocation ===\n";

    // These live on the stack — automatically destroyed when scope ends.
    int   a = 10;
    float b = 3.14f;
    std::array<int, 5> arr = {1, 2, 3, 4, 5};

    std::cout << "a lives at: " << &a << "  value=" << a << "\n";
    std::cout << "b lives at: " << &b << "  value=" << b << "\n";
    std::cout << "arr[0] at:  " << &arr[0] << "\n";

    // 'arr' is gone after this closing brace — no leak possible.
}

// ── 2. Heap allocation ───────────────────────────────────────────────────────
void heap_demo() {
    std::cout << "\n=== Heap Allocation (raw new/delete) ===\n";

    int* p = new int(42);           // allocate one int on heap
    std::cout << "heap int at: " << p << "  value=" << *p << "\n";
    delete p;                       // must free manually
    p = nullptr;                    // best practice: null after delete

    int* arr = new int[5]{10, 20, 30, 40, 50};
    std::cout << "heap arr[2] = " << arr[2] << "\n";
    delete[] arr;                   // array form needs delete[]
    arr = nullptr;
}

// ── 3. When heap beats stack ─────────────────────────────────────────────────
void large_allocation_demo() {
    std::cout << "\n=== Large Allocation (heap wins) ===\n";

    // Putting 10 MB on the stack would likely crash (stack overflow).
    // On the heap it's fine:
    constexpr std::size_t N = 10'000'000;
    std::vector<int> big(N, 0);    // std::vector manages heap internally
    big[N - 1] = 99;
    std::cout << "Allocated " << N << " ints on heap via vector. Last=" << big[N-1] << "\n";
}

// ── 4. Performance micro-benchmark ──────────────────────────────────────────
void perf_demo() {
    std::cout << "\n=== Stack vs Heap Speed ===\n";
    constexpr int ITER = 1'000'000;

    // Stack
    auto t0 = std::chrono::high_resolution_clock::now();
    volatile long long stack_sum = 0;
    for (int i = 0; i < ITER; ++i) {
        int x = i;                  // stack alloc each iteration
        stack_sum += x;
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    // Heap
    volatile long long heap_sum = 0;
    for (int i = 0; i < ITER; ++i) {
        int* x = new int(i);        // heap alloc each iteration (expensive!)
        heap_sum += *x;
        delete x;
    }
    auto t2 = std::chrono::high_resolution_clock::now();

    using ms = std::chrono::microseconds;
    std::cout << "Stack sum time: "
              << std::chrono::duration_cast<ms>(t1 - t0).count() << " µs\n";
    std::cout << "Heap  sum time: "
              << std::chrono::duration_cast<ms>(t2 - t1).count() << " µs\n";
    std::cout << "(heap is slower due to allocator overhead)\n";
}

// ── 5. Stack frame diagram (textual) ────────────────────────────────────────
void explain_stack_frame() {
    std::cout << R"(
=== Stack Frame Mental Model ===
  High address
  ┌─────────────────┐
  │   main() frame  │  ← first frame
  ├─────────────────┤
  │  stack_demo()   │  ← called by main
  │   a=10, b=3.14  │
  │   arr[5]        │
  ├─────────────────┤
  │  explain_...()  │  ← called by main (current)
  └─────────────────┘
  Low address  (stack grows downward on x86)

  Heap lives at a completely different memory region,
  managed by the allocator (malloc/free or new/delete).
)";
}

int main() {
    stack_demo();
    heap_demo();
    large_allocation_demo();
    perf_demo();
    explain_stack_frame();

    std::cout << "\n✓ Lesson 01 complete.\n";
}
