/**
 * LESSON 06 — Virtual Functions and Polymorphism
 * ================================================
 * Key ideas:
 *  - virtual enables runtime (dynamic) dispatch via vtable.
 *  - override keyword verifies you are actually overriding a base method.
 *  - final prevents further overriding / inheritance.
 *  - Pure virtual (= 0) makes a class abstract.
 *  - Virtual destructor: ALWAYS declare it in polymorphic base classes.
 *  - Covariant return types.
 *  - Dynamic cast for safe downcasting.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <typeinfo>
#include <cassert>
#include <cmath>

// ── 1. Abstract base class ───────────────────────────────────────────────────
class Shape {
public:
    virtual ~Shape() = default;         // virtual dtor — essential!

    virtual double area()      const = 0;   // pure virtual
    virtual double perimeter() const = 0;   // pure virtual
    virtual std::string name() const = 0;   // pure virtual

    // Non-virtual helper that uses virtual methods
    void describe() const {
        std::cout << "  " << name()
                  << " | area=" << area()
                  << " | perimeter=" << perimeter() << "\n";
    }

    // Virtual clone — covariant return type pattern
    virtual Shape* clone() const = 0;
};

// ── 2. Concrete derived classes ──────────────────────────────────────────────
class Circle : public Shape {
public:
    explicit Circle(double r) : radius_(r) {}

    double area()      const override { return M_PI * radius_ * radius_; }
    double perimeter() const override { return 2.0 * M_PI * radius_;     }
    std::string name() const override { return "Circle(r=" + std::to_string(radius_) + ")"; }
    Circle* clone()    const override { return new Circle(*this); }   // covariant

private:
    double radius_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width_(w), height_(h) {}

    double area()      const override { return width_ * height_;          }
    double perimeter() const override { return 2.0 * (width_ + height_);  }
    std::string name() const override { return "Rect(" + std::to_string(width_)
                                              + "x" + std::to_string(height_) + ")"; }
    Rectangle* clone() const override { return new Rectangle(*this); }

private:
    double width_, height_;
};

class Triangle : public Shape {
public:
    Triangle(double a, double b, double c) : a_(a), b_(b), c_(c) {}

    double perimeter() const override { return a_ + b_ + c_; }
    double area()      const override {
        double s = perimeter() / 2.0;
        return std::sqrt(s * (s-a_) * (s-b_) * (s-c_));  // Heron's formula
    }
    std::string name() const override { return "Triangle"; }
    Triangle*   clone() const override { return new Triangle(*this); }

private:
    double a_, b_, c_;
};

// ── 3. Polymorphic collection ────────────────────────────────────────────────
void polymorphic_collection() {
    std::cout << "\n=== Polymorphic Collection ===\n";

    // Store by base-class pointer — runtime dispatch via vtable
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));

    for (const auto& s : shapes) s->describe();

    // Find largest area
    double maxA = 0;
    for (const auto& s : shapes) maxA = std::max(maxA, s->area());
    std::cout << "  Largest area: " << maxA << "\n";
}

// ── 4. dynamic_cast / typeid ─────────────────────────────────────────────────
void dynamic_cast_demo() {
    std::cout << "\n=== dynamic_cast ===\n";

    std::unique_ptr<Shape> s = std::make_unique<Circle>(3.0);

    // Safe downcast — returns nullptr if wrong type
    if (auto* c = dynamic_cast<Circle*>(s.get())) {
        std::cout << "  dynamic_cast<Circle*> succeeded — it IS a Circle\n";
    } else {
        std::cout << "  not a Circle\n";
    }

    if (auto* r = dynamic_cast<Rectangle*>(s.get())) {
        std::cout << "  it's a Rectangle\n";
    } else {
        std::cout << "  dynamic_cast<Rectangle*> failed — not a Rectangle\n";
    }

    // typeid
    std::cout << "  typeid: " << typeid(*s).name() << "\n";
}

// ── 5. Without virtual (broken polymorphism) ─────────────────────────────────
struct Base {
    std::string type()          { return "Base"; }         // NOT virtual
    virtual std::string vtype() { return "Base"; }         // virtual
};
struct Derived : Base {
    std::string type()          { return "Derived"; }      // hides, does not override
    std::string vtype() override{ return "Derived"; }      // truly overrides
};

void virtual_vs_nonvirtual() {
    std::cout << "\n=== virtual vs non-virtual dispatch ===\n";
    Base* ptr = new Derived();

    std::cout << "  ptr->type()  (non-virtual): " << ptr->type()  << "\n"; // "Base"  ← WRONG
    std::cout << "  ptr->vtype() (virtual):     " << ptr->vtype() << "\n"; // "Derived" ← correct

    delete ptr;
}

// ── 6. Virtual destructor importance ─────────────────────────────────────────
struct BadBase  { ~BadBase()  { std::cout << "  ~BadBase\n";  } };
struct BadDerived : BadBase {
    std::string* data_ = new std::string("leak me");
    ~BadDerived() { delete data_; std::cout << "  ~BadDerived\n"; }
};

struct GoodBase  { virtual ~GoodBase()  { std::cout << "  ~GoodBase\n";  } };
struct GoodDerived : GoodBase {
    std::string* data_ = new std::string("properly freed");
    ~GoodDerived() { delete data_; std::cout << "  ~GoodDerived\n"; }
};

void virtual_destructor_demo() {
    std::cout << "\n=== Virtual Destructor ===\n";

    std::cout << "  BAD (no virtual dtor — derived dtor NOT called):\n";
    BadBase* bad = new BadDerived();
    delete bad;     // only ~BadBase called — data_ LEAKS

    std::cout << "  GOOD (virtual dtor — both dtors called):\n";
    GoodBase* good = new GoodDerived();
    delete good;    // ~GoodDerived then ~GoodBase — no leak
}

// ── 7. Interface (pure-virtual only) pattern ─────────────────────────────────
class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual std::string serialize()          const = 0;
    virtual void        deserialize(std::string_view) = 0;
};

class Config : public ISerializable {
public:
    Config(std::string k, std::string v) : key_(k), val_(v) {}
    std::string serialize()          const override { return key_ + "=" + val_; }
    void        deserialize(std::string_view s) override {
        auto pos = s.find('=');
        if (pos != std::string_view::npos) {
            key_ = s.substr(0, pos);
            val_ = s.substr(pos + 1);
        }
    }
    void print() const { std::cout << "  Config: " << key_ << "=" << val_ << "\n"; }
private:
    std::string key_, val_;
};

void interface_demo() {
    std::cout << "\n=== Interface Pattern ===\n";
    Config cfg("theme", "dark");
    cfg.print();
    std::cout << "  serialized: " << cfg.serialize() << "\n";
    cfg.deserialize("lang=cpp17");
    cfg.print();
}

int main() {
    polymorphic_collection();
    dynamic_cast_demo();
    virtual_vs_nonvirtual();
    virtual_destructor_demo();
    interface_demo();

    std::cout << "\n✓ Lesson 06 complete.\n";
}
