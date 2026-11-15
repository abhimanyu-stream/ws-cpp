/**
 * TOPIC 10 — Memory Management & Smart Pointers
 * ===============================================
 * Covers: stack/heap/static memory, RAII, unique_ptr, shared_ptr,
 *         weak_ptr, make_unique, make_shared, custom deleters,
 *         circular references, ownership transfer, move semantics
 *
 * Compile: g++ -std=c++17 -Wall 10_memory_smart_pointers.cpp -o memory
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

// ── Memory Regions ────────────────────────────────────────────────────────────
int global_var = 10;          // Static storage — lives for entire program
static int static_var = 20;  // Static storage

void section_memory_layout() {
    std::cout << "\n=== Memory Layout ===\n";

    // Stack — fast, automatic, limited size (~1-8 MB)
    int    local  = 42;        // stack
    char   buf[1024]{};        // stack — 1 KB

    // Heap — large, manual (or smart ptr), slower allocation
    int*   heap_i = new int(99);   // heap
    delete heap_i;

    // Static — single instance, persists
    static int s = 0;
    ++s;

    std::cout << "global:  " << global_var << " at " << &global_var << "\n";
    std::cout << "local:   " << local      << " at " << &local      << "\n";
    std::cout << "static:  " << s          << "\n";
    (void)buf;
}

// ── RAII — Resource Acquisition Is Initialization ────────────────────────────
// The object acquires a resource in its constructor and releases it in destructor.
// Guarantees cleanup even when exceptions are thrown.
class FileGuard {
public:
    explicit FileGuard(const char* path) : fp_(fopen(path, "r")) {
        if (fp_) std::cout << "  FileGuard: opened " << path << "\n";
    }
    ~FileGuard() {
        if (fp_) { fclose(fp_); std::cout << "  FileGuard: closed\n"; }
    }
    bool is_open() const { return fp_ != nullptr; }
    // Non-copyable, movable
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;
private:
    FILE* fp_;
};

void section_raii() {
    std::cout << "\n=== RAII ===\n";
    {
        FileGuard f("CMakeLists.txt");
        std::cout << "  open? " << f.is_open() << "\n";
    }   // destructor called here — file closed automatically
    std::cout << "  (FileGuard destroyed, file closed)\n";
}

// ── unique_ptr — sole ownership ───────────────────────────────────────────────
struct Resource {
    std::string name;
    explicit Resource(std::string n) : name(std::move(n)) {
        std::cout << "  [" << name << " created]\n";
    }
    ~Resource() { std::cout << "  [" << name << " destroyed]\n"; }
    void use() const { std::cout << "  using " << name << "\n"; }
};

void section_unique_ptr() {
    std::cout << "\n=== unique_ptr ===\n";

    // Prefer make_unique — exception-safe, no raw new
    auto r1 = std::make_unique<Resource>("R1");
    r1->use();

    // unique_ptr is NOT copyable — sole ownership
    // auto r2 = r1;      // ERROR — deleted copy constructor

    // Transfer ownership with std::move — r1 becomes null
    auto r2 = std::move(r1);
    std::cout << "r1 is " << (r1 ? "alive" : "null") << "\n";
    r2->use();

    // Reset — destroys current and optionally takes new ownership
    r2.reset(new Resource("R2"));   // R1 destroyed here
    r2.reset();                     // R2 destroyed here, r2 is null

    // get() — raw pointer (non-owning)
    auto r3 = std::make_unique<Resource>("R3");
    Resource* raw = r3.get();   // safe as long as r3 is alive
    raw->use();

    // release() — give up ownership without destroying
    Resource* raw2 = r3.release();   // r3 is now null, YOU must delete raw2
    delete raw2;

    // Factory function
    auto make_resource = [](std::string n) -> std::unique_ptr<Resource> {
        return std::make_unique<Resource>(std::move(n));
    };
    auto r4 = make_resource("R4");
    r4->use();

    // unique_ptr with array
    auto arr = std::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) arr[i] = i*i;
    std::cout << "arr[3] = " << arr[3] << "\n";   // 9

    std::cout << "--- leaving unique_ptr section ---\n";
}   // r2, r3, r4, arr all destroyed automatically

// ── shared_ptr — shared ownership via reference counting ─────────────────────
void section_shared_ptr() {
    std::cout << "\n=== shared_ptr ===\n";

    auto sp1 = std::make_shared<Resource>("Shared");
    std::cout << "use_count: " << sp1.use_count() << "\n";   // 1

    {
        auto sp2 = sp1;    // copy — both share ownership
        std::cout << "use_count after copy: " << sp1.use_count() << "\n";  // 2
        sp2->use();
    }   // sp2 destroyed — count back to 1

    std::cout << "use_count after sp2 scope: " << sp1.use_count() << "\n"; // 1

    // Passing by value increments the count temporarily
    auto consume = [](std::shared_ptr<Resource> sp) {
        std::cout << "  inside consume, count=" << sp.use_count() << "\n";  // 2
    };
    consume(sp1);
    std::cout << "use_count after consume: " << sp1.use_count() << "\n";   // 1

    // Vector of shared_ptrs — multiple owners
    std::vector<std::shared_ptr<Resource>> owners;
    owners.push_back(sp1);
    owners.push_back(sp1);
    std::cout << "use_count with 2 owners: " << sp1.use_count() << "\n";   // 3

    std::cout << "--- leaving shared_ptr section ---\n";
}   // sp1 and owners destroyed — Shared resource finally freed

// ── weak_ptr — non-owning observer, breaks cycles ─────────────────────────────
struct Node {
    std::string name;
    std::shared_ptr<Node> next;    // strong ref  → could cause cycle
    std::weak_ptr<Node>   prev;    // weak ref    → breaks cycle
    explicit Node(std::string n) : name(std::move(n)) {
        std::cout << "  [Node " << name << " created]\n";
    }
    ~Node() { std::cout << "  [Node " << name << " destroyed]\n"; }
};

void section_weak_ptr() {
    std::cout << "\n=== weak_ptr ===\n";

    auto n1 = std::make_shared<Node>("A");
    auto n2 = std::make_shared<Node>("B");

    n1->next = n2;    // A ──(strong)──► B
    n2->prev = n1;    // B ──(weak)────► A   (doesn't bump count)

    std::cout << "n1 count=" << n1.use_count() << "\n";  // 1 (weak doesn't count)
    std::cout << "n2 count=" << n2.use_count() << "\n";  // 2 (n1->next + n2)

    // Promote weak_ptr → shared_ptr for safe access
    if (auto locked = n2->prev.lock()) {
        std::cout << "n2->prev is " << locked->name << "\n";
    }

    // After n1 and n2 go out of scope → both Nodes properly destroyed
    // If we had used shared_ptr for prev → cycle → neither destroyed (memory leak)
    std::cout << "--- leaving weak_ptr section ---\n";
}

// ── Custom Deleters ───────────────────────────────────────────────────────────
void section_custom_deleter() {
    std::cout << "\n=== Custom Deleters ===\n";

    // unique_ptr with lambda deleter — for C resources
    auto file_del = [](FILE* f) {
        if (f) { fclose(f); std::cout << "  [FILE closed]\n"; }
    };
    std::unique_ptr<FILE, decltype(file_del)>
        fp(fopen("CMakeLists.txt", "r"), file_del);

    if (fp) std::cout << "  File opened, will auto-close\n";
    else    std::cout << "  File not found (demo still works)\n";

    // shared_ptr with deleter function
    auto arr_del = [](int* p) {
        std::cout << "  [array deleted]\n";
        delete[] p;
    };
    std::shared_ptr<int> arr(new int[10], arr_del);
    arr.get()[0] = 42;
    std::cout << "  arr[0]=" << arr.get()[0] << "\n";
}

// ── Ownership Semantics ───────────────────────────────────────────────────────
// Rule of thumb for function signatures:

// Takes ownership (sink) — caller gives up the resource
void sink(std::unique_ptr<Resource> r) {
    r->use();
}   // r destroyed here

// Borrows (read-only) — doesn't affect ownership
void borrow_const(const Resource& r) {
    r.use();
}

// Borrows (mutable) — doesn't affect ownership
void borrow(Resource& r) {
    r.use();
}

// Shares ownership
void share(std::shared_ptr<Resource> r) {
    r->use();   // r keeps resource alive while this function runs
}

void section_ownership() {
    std::cout << "\n=== Ownership Semantics ===\n";

    auto u = std::make_unique<Resource>("Owned");
    borrow_const(*u);      // OK — still owns
    borrow(*u);            // OK — still owns
    sink(std::move(u));    // ownership transferred — u is now null
    // u->use();           // ERROR — u is null

    auto s = std::make_shared<Resource>("Shared2");
    share(s);              // count temporarily 2 during call
    std::cout << "count after share: " << s.use_count() << "\n";   // 1
}

int main() {
    std::cout << "╔══ Topic 10: Memory & Smart Pointers ══╗\n";

    section_memory_layout();
    section_raii();
    section_unique_ptr();
    section_shared_ptr();
    section_weak_ptr();
    section_custom_deleter();
    section_ownership();

    std::cout << "\n✓ Topic 10 complete.\n";
}
