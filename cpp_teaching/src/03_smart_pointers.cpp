/**
 * LESSON 03 — Smart Pointers: unique_ptr, shared_ptr, weak_ptr
 * =============================================================
 * Key ideas:
 *  - Smart pointers automate lifetime management (RAII over raw new/delete).
 *  - unique_ptr  : sole ownership, zero overhead vs raw pointer, non-copyable.
 *  - shared_ptr  : shared ownership via reference counting, slight overhead.
 *  - weak_ptr    : non-owning observer of a shared_ptr; breaks cyclic refs.
 *  - Prefer make_unique / make_shared over calling new directly.
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

// ── Helper type ──────────────────────────────────────────────────────────────
struct Animal {
    std::string name;
    explicit Animal(std::string n) : name(std::move(n)) {
        std::cout << "  [Animal '" << name << "' born]\n";
    }
    ~Animal() {
        std::cout << "  [Animal '" << name << "' died]\n";
    }
    void speak() const { std::cout << "  " << name << " says: ...\n"; }
};

// ── 1. unique_ptr ─────────────────────────────────────────────────────────────
void unique_ptr_demo() {
    std::cout << "\n=== unique_ptr ===\n";

    // Creation — prefer make_unique (exception-safe, no raw new)
    auto dog = std::make_unique<Animal>("Rex");
    dog->speak();

    // Ownership transfer via std::move (dog becomes null after move)
    auto dog2 = std::move(dog);
    std::cout << "dog  is " << (dog  ? "alive" : "null (transferred)") << "\n";
    std::cout << "dog2 is " << (dog2 ? "alive" : "null") << "\n";
    dog2->speak();

    // Factory function pattern
    auto make_animal = [](std::string n) -> std::unique_ptr<Animal> {
        return std::make_unique<Animal>(std::move(n));
    };
    auto cat = make_animal("Whiskers");
    cat->speak();

    // Storing in a vector
    std::vector<std::unique_ptr<Animal>> zoo;
    zoo.push_back(std::make_unique<Animal>("Lion"));
    zoo.push_back(std::make_unique<Animal>("Tiger"));
    for (auto& a : zoo) a->speak();

    std::cout << "--- leaving unique_ptr_demo scope: all animals destroyed ---\n";
}   // dog2, cat, zoo all destroyed here automatically

// ── 2. shared_ptr ─────────────────────────────────────────────────────────────
void shared_ptr_demo() {
    std::cout << "\n=== shared_ptr ===\n";

    auto sp1 = std::make_shared<Animal>("Parrot");
    std::cout << "use_count after sp1: " << sp1.use_count() << "\n";  // 1

    {
        auto sp2 = sp1;     // copy — both share ownership
        std::cout << "use_count after sp2: " << sp1.use_count() << "\n";  // 2
        sp2->speak();
    }   // sp2 destroyed — count back to 1

    std::cout << "use_count after sp2 scope: " << sp1.use_count() << "\n";  // 1

    // Passing shared_ptr to a function (observe count changes)
    auto consume = [](std::shared_ptr<Animal> sp) {
        std::cout << "  inside consume, use_count=" << sp.use_count() << "\n";
    };
    consume(sp1);   // count temporarily 2 during call
    std::cout << "use_count after consume: " << sp1.use_count() << "\n";  // 1

    std::cout << "--- leaving shared_ptr_demo: Parrot destroyed ---\n";
}

// ── 3. weak_ptr and cycle breaking ──────────────────────────────────────────
struct Node {
    std::string name;
    std::shared_ptr<Node> next;    // strong ref  → could cause a cycle
    std::weak_ptr<Node>   prev;    // weak ref    → breaks cycle
    explicit Node(std::string n) : name(std::move(n)) {
        std::cout << "  [Node '" << name << "' created]\n";
    }
    ~Node() { std::cout << "  [Node '" << name << "' destroyed]\n"; }
};

void weak_ptr_demo() {
    std::cout << "\n=== weak_ptr (cycle breaking) ===\n";

    auto n1 = std::make_shared<Node>("A");
    auto n2 = std::make_shared<Node>("B");

    n1->next = n2;        // A ──(strong)──► B
    n2->prev = n1;        // B ──(weak)────► A   (no ref-count bump)

    std::cout << "n1 use_count=" << n1.use_count() << "\n";  // 1 (n2->prev is weak)
    std::cout << "n2 use_count=" << n2.use_count() << "\n";  // 2 (n1->next + n2)

    // Promote weak_ptr to shared_ptr for safe access
    if (auto locked = n2->prev.lock()) {
        std::cout << "n2->prev points to: " << locked->name << "\n";
    }

    std::cout << "--- scope ends: both nodes properly destroyed (no leak) ---\n";
}

// ── 4. Pitfalls ──────────────────────────────────────────────────────────────
void pitfalls() {
    std::cout << "\n=== Common Pitfalls ===\n";

    // ✗ BAD: constructing two independent shared_ptrs from the same raw pointer
    // Animal* raw = new Animal("Bug");
    // std::shared_ptr<Animal> s1(raw);
    // std::shared_ptr<Animal> s2(raw);  // double-free UB!

    // ✓ GOOD: always use make_shared or share via copy
    auto s1 = std::make_shared<Animal>("Safe");
    auto s2 = s1;   // shares ownership correctly
    std::cout << "use_count=" << s1.use_count() << "\n";

    // ✗ BAD: storing raw pointer from unique_ptr and using after move
    auto up = std::make_unique<Animal>("Temp");
    // Animal* raw = up.get();
    // auto up2 = std::move(up);
    // raw->speak();  // UB — raw still points to memory, up2 owns it now

    // ✓ GOOD: use get() only within a known lifetime window
    up->speak();    // safe — up still owns it
}

// ── 5. Custom deleter ────────────────────────────────────────────────────────
void custom_deleter_demo() {
    std::cout << "\n=== Custom Deleter ===\n";

    // Useful for C resources (FILE*, SDL_Surface*, etc.)
    auto file_deleter = [](FILE* f) {
        if (f) { fclose(f); std::cout << "  [FILE closed by custom deleter]\n"; }
    };

    std::unique_ptr<FILE, decltype(file_deleter)>
        fp(fopen("CMakeLists.txt", "r"), file_deleter);

    if (fp) std::cout << "  File opened & will auto-close.\n";
    else    std::cout << "  File not found (that's ok, demo still works).\n";
}

int main() {
    unique_ptr_demo();
    shared_ptr_demo();
    weak_ptr_demo();
    pitfalls();
    custom_deleter_demo();

    std::cout << "\n✓ Lesson 03 complete.\n";
}
