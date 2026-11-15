/**
 * TOPIC 07 — Object-Oriented Programming (OOP)
 * ==============================================
 * Covers: classes, access specifiers, encapsulation, constructors,
 *         destructors, inheritance, polymorphism, virtual functions,
 *         abstract classes, operator overloading, diamond problem,
 *         Rule of Three/Five/Zero, object slicing, this pointer,
 *         friend, nested classes, static members
 *
 * Compile: g++ -std=c++17 -Wall 07_oop.cpp -o oop
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ═══════════════════════════════════════════════════════════════════════════════
// 1.  Classes, Access Specifiers, Encapsulation
// ═══════════════════════════════════════════════════════════════════════════════

class BankAccount {
public:
    // Public interface — what the outside world can see
    BankAccount(std::string owner, double balance = 0.0)
        : owner_(std::move(owner)), balance_(balance) {}

    void deposit(double amount) {
        if (amount > 0) balance_ += amount;
    }

    bool withdraw(double amount) {
        if (amount > 0 && amount <= balance_) {
            balance_ -= amount;
            return true;
        }
        return false;
    }

    double get_balance() const { return balance_; }  // const — read-only
    const std::string& owner() const { return owner_; }

    void print() const {
        std::cout << owner_ << ": $" << balance_ << "\n";
    }

private:
    // Private data — only accessible by this class
    std::string owner_;
    double      balance_;

protected:
    // Protected — accessible by this class and derived classes
    void set_balance(double b) { balance_ = b; }
};

void section_classes() {
    std::cout << "\n=== Classes & Encapsulation ===\n";

    BankAccount acc("Alice", 1000.0);
    acc.deposit(500.0);
    acc.withdraw(200.0);
    acc.print();   // Alice: $1300
    // acc.balance_ = 9999;  // ERROR — private member
}

// ═══════════════════════════════════════════════════════════════════════════════
// 2.  Constructors & Destructor
// ═══════════════════════════════════════════════════════════════════════════════

class Widget {
public:
    // Default constructor
    Widget() : id_(++count_), name_("unnamed") {
        std::cout << "  Widget() default #" << id_ << "\n";
    }

    // Parameterized constructor
    explicit Widget(std::string name)
        : id_(++count_), name_(std::move(name)) {
        std::cout << "  Widget(name) #" << id_ << " '" << name_ << "'\n";
    }

    // Copy constructor — deep copy of members
    Widget(const Widget& other)
        : id_(++count_), name_(other.name_ + "_copy") {
        std::cout << "  Widget(copy) #" << id_ << " from #" << other.id_ << "\n";
    }

    // Move constructor — steal resources from temporary
    Widget(Widget&& other) noexcept
        : id_(other.id_), name_(std::move(other.name_)) {
        other.id_   = 0;
        other.name_ = "(moved)";
        std::cout << "  Widget(move) #" << id_ << "\n";
    }

    // Copy assignment
    Widget& operator=(const Widget& other) {
        if (this != &other) name_ = other.name_ + "_assigned";
        std::cout << "  Widget operator=(copy)\n";
        return *this;
    }

    // Move assignment
    Widget& operator=(Widget&& other) noexcept {
        if (this != &other) {
            name_       = std::move(other.name_);
            other.name_ = "(moved)";
        }
        std::cout << "  Widget operator=(move)\n";
        return *this;
    }

    // Destructor
    ~Widget() {
        std::cout << "  ~Widget #" << id_ << " '" << name_ << "'\n";
    }

    // Constructor delegation (C++11) — one constructor calls another
    Widget(int) : Widget("from-int") {
        std::cout << "  Widget(int) delegated\n";
    }

    std::string name() const { return name_; }
    int         id()   const { return id_;   }

    static int total() { return count_; }

private:
    int         id_;
    std::string name_;
    static int  count_;  // shared across all instances
};

int Widget::count_ = 0;

void section_constructors() {
    std::cout << "\n=== Constructors & Destructor ===\n";
    {
        Widget a;                   // default
        Widget b("Button");         // parameterized
        Widget c = b;               // copy
        Widget d = std::move(b);    // move
        Widget e(42);               // delegating constructor
        a = c;                      // copy assignment
        a = std::move(d);           // move assignment
    }   // all destroyed here
    std::cout << "Total widgets created: " << Widget::total() << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 3.  Inheritance
// ═══════════════════════════════════════════════════════════════════════════════

class Animal {
public:
    explicit Animal(std::string name) : name_(std::move(name)) {}
    virtual ~Animal() = default;   // ALWAYS make base destructor virtual

    std::string name() const { return name_; }
    virtual void speak() const { std::cout << name_ << ": ...\n"; }
    void breathe() const       { std::cout << name_ << " breathes\n"; }

protected:
    std::string name_;
};

class Dog : public Animal {
public:
    explicit Dog(std::string name, std::string breed)
        : Animal(std::move(name)), breed_(std::move(breed)) {}

    void speak() const override {   // override keyword — compiler checks this
        std::cout << name_ << " (" << breed_ << "): Woof!\n";
    }

    void fetch() const { std::cout << name_ << " fetches!\n"; }

private:
    std::string breed_;
};

class Cat : public Animal {
public:
    explicit Cat(std::string name) : Animal(std::move(name)) {}

    void speak() const override {
        std::cout << name_ << ": Meow!\n";
    }
};

// Multilevel inheritance
class GuideDog : public Dog {
public:
    GuideDog(std::string name, std::string breed, std::string owner)
        : Dog(std::move(name), std::move(breed)), owner_(std::move(owner)) {}

    void guide() const {
        std::cout << name_ << " guides " << owner_ << "\n";
    }

private:
    std::string owner_;
};

void section_inheritance() {
    std::cout << "\n=== Inheritance ===\n";

    Dog  rex("Rex",     "Labrador");
    Cat  whiskers("Whiskers");
    GuideDog buddy("Buddy", "Golden", "Alice");

    rex.speak();
    whiskers.speak();
    buddy.speak();
    buddy.guide();
    buddy.fetch();

    // Polymorphism via base pointer
    Animal* animals[] = { &rex, &whiskers, &buddy };
    for (auto* a : animals) a->speak();   // correct overridden version called each time
}

// ═══════════════════════════════════════════════════════════════════════════════
// 4.  Virtual Functions, Abstract Classes, Pure Virtual
// ═══════════════════════════════════════════════════════════════════════════════

// Abstract class — cannot be instantiated, only used as base
class Shape {
public:
    virtual double area()      const = 0;   // pure virtual — MUST be overridden
    virtual double perimeter() const = 0;
    virtual void   describe()  const {      // non-pure virtual — optional override
        std::cout << "Shape: area=" << area()
                  << " perimeter=" << perimeter() << "\n";
    }
    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area()      const override { return 3.14159 * r_ * r_; }
    double perimeter() const override { return 2 * 3.14159 * r_; }
private:
    double r_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area()      const override { return w_ * h_; }
    double perimeter() const override { return 2 * (w_ + h_); }
private:
    double w_, h_;
};

// Object slicing — data lost when copying derived to base by value
void section_virtual() {
    std::cout << "\n=== Virtual / Abstract / Slicing ===\n";

    // Shape s;   // ERROR — abstract class

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));

    for (const auto& s : shapes) s->describe();

    // Object slicing — derived data is LOST
    Dog  d("Rex", "Lab");
    Animal a = d;      // sliced — only Animal part copied, Dog part gone
    a.speak();         // calls Animal::speak() NOT Dog::speak()
    std::cout << "(object slicing: derived part lost)\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5.  Operator Overloading
// ═══════════════════════════════════════════════════════════════════════════════

class Vec2 {
public:
    double x, y;
    Vec2(double x=0, double y=0) : x(x), y(y) {}

    // Arithmetic
    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2 operator*(double s)      const { return {x*s,   y*s};   }

    // Compound assignment
    Vec2& operator+=(const Vec2& o) { x+=o.x; y+=o.y; return *this; }

    // Comparison (C++20 auto-generates !=, <, >, <=, >= from these)
    bool operator==(const Vec2& o) const { return x==o.x && y==o.y; }

    // Subscript
    double& operator[](int i) { return i==0 ? x : y; }

    // Unary negation
    Vec2 operator-() const { return {-x, -y}; }

    // Stream insertion (non-member friend for symmetric usage)
    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        return os << "(" << v.x << ", " << v.y << ")";
    }

    // Prefix ++
    Vec2& operator++()    { ++x; ++y; return *this; }
    // Postfix ++ (int dummy parameter distinguishes from prefix)
    Vec2  operator++(int) { Vec2 tmp=*this; ++x; ++y; return tmp; }
};

// Non-member: allows  scalar * Vec2  not just  Vec2 * scalar
Vec2 operator*(double s, const Vec2& v) { return v * s; }

void section_operators() {
    std::cout << "\n=== Operator Overloading ===\n";

    Vec2 a{1, 2}, b{3, 4};
    std::cout << "a+b = " << (a+b) << "\n";
    std::cout << "a-b = " << (a-b) << "\n";
    std::cout << "a*2 = " << (a*2) << "\n";
    std::cout << "3*b = " << (3.0*b) << "\n";   // non-member
    std::cout << "-a  = " << (-a)   << "\n";
    std::cout << "a[0]= " << a[0]   << "\n";
    std::cout << "a==b: " << (a==b) << "\n";
    ++a;
    std::cout << "++a = " << a << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 6.  Static Members, this pointer, Friend
// ═══════════════════════════════════════════════════════════════════════════════

class Counter {
public:
    Counter()  { ++total_; }
    ~Counter() { --total_; }

    static int total() { return total_; }  // static — no 'this', shared state

    // 'this' — pointer to the current object
    Counter& set_name(const std::string& n) {
        name_ = n;
        return *this;   // return *this enables method chaining
    }
    Counter& set_value(int v) {
        value_ = v;
        return *this;
    }
    void print() const {
        std::cout << "Counter[" << name_ << "]=" << value_ << "\n";
    }

    // Friend function — can access private members of Counter
    friend void inspect(const Counter& c);

private:
    std::string name_  = "?";
    int         value_ = 0;
    static int  total_;
};
int Counter::total_ = 0;

void inspect(const Counter& c) {
    // Can access private members because it's a friend
    std::cout << "inspect: name='" << c.name_ << "' val=" << c.value_ << "\n";
}

void section_static_this_friend() {
    std::cout << "\n=== Static, this, Friend ===\n";

    Counter a, b, c;
    std::cout << "total: " << Counter::total() << "\n";   // 3

    // Method chaining via return *this
    a.set_name("alpha").set_value(42).print();

    inspect(b);   // friend function
}

// ═══════════════════════════════════════════════════════════════════════════════
// 7.  Diamond Problem & Virtual Inheritance
// ═══════════════════════════════════════════════════════════════════════════════

struct Base {
    int value = 0;
    virtual void who() const { std::cout << "Base\n"; }
    virtual ~Base() = default;
};

// Without virtual inheritance, Derived would have TWO copies of Base:
//   Left::Base  and  Right::Base — ambiguity!

struct Left  : virtual Base { void who() const override { std::cout << "Left\n"; } };
struct Right : virtual Base { void who() const override { std::cout << "Right\n"; } };

struct Diamond : Left, Right {
    void who() const override { std::cout << "Diamond\n"; }
};

void section_diamond() {
    std::cout << "\n=== Diamond Problem ===\n";

    Diamond d;
    d.value = 42;    // OK — only ONE copy of Base::value with virtual inheritance
    d.who();         // Diamond

    // Without virtual: d.value would be ambiguous (d.Left::value vs d.Right::value)
    std::cout << "diamond Base::value = " << d.value << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// 8.  Rule of Zero / Three / Five
// ═══════════════════════════════════════════════════════════════════════════════

// Rule of Zero — let the compiler generate everything (preferred)
struct PersonZero {
    std::string name;    // string manages its own resources
    int         age{};
    // No destructor, no copy/move — compiler generates correct ones
};

// Rule of Three (pre-C++11) — if you write destructor, write copy ctor + copy assign
// Rule of Five (C++11+)     — if you write any of the 5, write all 5
struct ResourceOwner {
    int* data;
    int  size;

    explicit ResourceOwner(int n) : data(new int[n]), size(n) {}
    ~ResourceOwner()                           { delete[] data; }
    ResourceOwner(const ResourceOwner& o)      : data(new int[o.size]), size(o.size)
                                               { std::copy(o.data, o.data+size, data); }
    ResourceOwner& operator=(const ResourceOwner& o) {
        if (this != &o) { delete[] data; size=o.size; data=new int[size]; std::copy(o.data,o.data+size,data); }
        return *this;
    }
    ResourceOwner(ResourceOwner&& o) noexcept  : data(o.data), size(o.size)
                                               { o.data=nullptr; o.size=0; }
    ResourceOwner& operator=(ResourceOwner&& o) noexcept {
        if (this != &o) { delete[] data; data=o.data; size=o.size; o.data=nullptr; o.size=0; }
        return *this;
    }
};

void section_rule_of_five() {
    std::cout << "\n=== Rule of Zero / Five ===\n";

    PersonZero p1{"Alice", 30};          // Rule of Zero
    PersonZero p2 = p1;                  // compiler-generated copy — works
    std::cout << p2.name << " " << p2.age << "\n";

    ResourceOwner r1(5);                 // Rule of Five
    for (int i=0; i<5; ++i) r1.data[i] = i;
    ResourceOwner r2 = r1;               // deep copy
    ResourceOwner r3 = std::move(r1);    // move — r1 now null
    std::cout << "r2[2]=" << r2.data[2] << " r3[4]=" << r3.data[4] << "\n";
}

int main() {
    std::cout << "╔══ Topic 07: OOP ══╗\n";

    section_classes();
    section_constructors();
    section_inheritance();
    section_virtual();
    section_operators();
    section_static_this_friend();
    section_diamond();
    section_rule_of_five();

    std::cout << "\n✓ Topic 07 complete.\n";
}
