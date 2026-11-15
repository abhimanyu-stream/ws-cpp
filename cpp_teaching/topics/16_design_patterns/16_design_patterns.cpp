/**
 * TOPIC 16 — Design Patterns in C++
 * ===================================
 * Covers: Creational, Structural, Behavioral patterns
 * Singleton, Factory, Builder, Adapter, Decorator, Proxy,
 * Observer, Strategy, Command, State, Iterator, RAII pattern
 *
 * Compile: g++ -std=c++17 -Wall 16_design_patterns.cpp -o patterns
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <stack>
#include <algorithm>

// ═══════════════════════════════════════════════════════════════════════════════
// CREATIONAL PATTERNS
// ═══════════════════════════════════════════════════════════════════════════════

// ── 1. Singleton ──────────────────────────────────────────────────────────────
// Ensures only ONE instance of a class exists.
// Use for: global configuration, logger, thread pool.
class Logger {
public:
    // Static method returns the single instance
    static Logger& instance() {
        static Logger inst;   // constructed on first call, destroyed at program end
        return inst;
    }

    void log(const std::string& msg) {
        std::cout << "[LOG] " << msg << "\n";
    }

    // Delete copy and move to prevent duplication
    Logger(const Logger&)            = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() { std::cout << "Logger created\n"; }
    ~Logger(){ std::cout << "Logger destroyed\n"; }
};

// ── 2. Factory Method ─────────────────────────────────────────────────────────
// Defines an interface for creating objects, lets subclasses decide the type.
struct Animal {
    virtual ~Animal() = default;
    virtual void speak() const = 0;
    virtual std::string type() const = 0;
};

struct Dog : Animal {
    void speak() const override { std::cout << "Woof!\n"; }
    std::string type() const override { return "Dog"; }
};
struct Cat : Animal {
    void speak() const override { std::cout << "Meow!\n"; }
    std::string type() const override { return "Cat"; }
};
struct Bird : Animal {
    void speak() const override { std::cout << "Tweet!\n"; }
    std::string type() const override { return "Bird"; }
};

// Factory function — creates objects by name without exposing the type
std::unique_ptr<Animal> make_animal(const std::string& kind) {
    if (kind == "dog")  return std::make_unique<Dog>();
    if (kind == "cat")  return std::make_unique<Cat>();
    if (kind == "bird") return std::make_unique<Bird>();
    return nullptr;
}

// Abstract Factory — creates families of related objects
struct Button    { virtual void render() const = 0; virtual ~Button() = default; };
struct Checkbox  { virtual void render() const = 0; virtual ~Checkbox() = default; };

struct WinButton  : Button   { void render() const override { std::cout << "[Win Button]\n"; } };
struct WinCheckbox: Checkbox { void render() const override { std::cout << "[Win Checkbox]\n"; } };
struct MacButton  : Button   { void render() const override { std::cout << "[Mac Button]\n"; } };
struct MacCheckbox: Checkbox { void render() const override { std::cout << "[Mac Checkbox]\n"; } };

struct UIFactory {
    virtual std::unique_ptr<Button>   create_button()   const = 0;
    virtual std::unique_ptr<Checkbox> create_checkbox()  const = 0;
    virtual ~UIFactory() = default;
};
struct WinFactory : UIFactory {
    std::unique_ptr<Button>   create_button()  const override { return std::make_unique<WinButton>(); }
    std::unique_ptr<Checkbox> create_checkbox()const override { return std::make_unique<WinCheckbox>(); }
};
struct MacFactory : UIFactory {
    std::unique_ptr<Button>   create_button()  const override { return std::make_unique<MacButton>(); }
    std::unique_ptr<Checkbox> create_checkbox()const override { return std::make_unique<MacCheckbox>(); }
};

// ── 3. Builder ────────────────────────────────────────────────────────────────
// Constructs complex objects step by step.
struct Pizza {
    std::string crust, sauce;
    std::vector<std::string> toppings;

    void print() const {
        std::cout << "Pizza[" << crust << "/" << sauce << "] toppings: ";
        for (const auto& t : toppings) std::cout << t << " ";
        std::cout << "\n";
    }
};

class PizzaBuilder {
public:
    PizzaBuilder& crust(std::string c)   { pizza_.crust = std::move(c);  return *this; }
    PizzaBuilder& sauce(std::string s)   { pizza_.sauce = std::move(s);  return *this; }
    PizzaBuilder& topping(std::string t) { pizza_.toppings.push_back(std::move(t)); return *this; }
    Pizza         build()                { return std::move(pizza_); }
private:
    Pizza pizza_;
};

// ── 4. Prototype ──────────────────────────────────────────────────────────────
// Clone an existing object instead of creating from scratch.
struct Config {
    std::string host;
    int         port;
    bool        secure;

    std::unique_ptr<Config> clone() const {
        return std::make_unique<Config>(*this);  // copy constructor
    }

    void print() const {
        std::cout << "Config{" << host << ":" << port << " secure=" << secure << "}\n";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// STRUCTURAL PATTERNS
// ═══════════════════════════════════════════════════════════════════════════════

// ── 5. Adapter ────────────────────────────────────────────────────────────────
// Wraps an incompatible interface to match a target interface.
struct LegacyPrinter {
    void print_old(const char* text) {
        std::cout << "[Legacy] " << text << "\n";
    }
};

struct Printer {
    virtual void print(const std::string& text) const = 0;
    virtual ~Printer() = default;
};

struct PrinterAdapter : Printer {
    explicit PrinterAdapter(LegacyPrinter& lp) : lp_(lp) {}
    void print(const std::string& text) const override {
        lp_.print_old(text.c_str());   // adapt new interface to old
    }
private:
    LegacyPrinter& lp_;
};

// ── 6. Decorator ──────────────────────────────────────────────────────────────
// Adds behavior to objects dynamically without subclassing.
struct TextWidget {
    virtual ~TextWidget() = default;
    virtual std::string render() const = 0;
};

struct PlainText : TextWidget {
    explicit PlainText(std::string t) : text_(std::move(t)) {}
    std::string render() const override { return text_; }
private:
    std::string text_;
};

struct BoldDecorator : TextWidget {
    explicit BoldDecorator(std::unique_ptr<TextWidget> w) : inner_(std::move(w)) {}
    std::string render() const override { return "<b>" + inner_->render() + "</b>"; }
private:
    std::unique_ptr<TextWidget> inner_;
};

struct ItalicDecorator : TextWidget {
    explicit ItalicDecorator(std::unique_ptr<TextWidget> w) : inner_(std::move(w)) {}
    std::string render() const override { return "<i>" + inner_->render() + "</i>"; }
private:
    std::unique_ptr<TextWidget> inner_;
};

// ── 7. Proxy ──────────────────────────────────────────────────────────────────
// Controls access to an object (lazy init, logging, caching, access control).
struct Image {
    virtual void display() const = 0;
    virtual ~Image() = default;
};

struct RealImage : Image {
    explicit RealImage(const std::string& file) : file_(file) {
        std::cout << "  [Loading " << file_ << " from disk...]\n";
    }
    void display() const override { std::cout << "  Displaying " << file_ << "\n"; }
private:
    std::string file_;
};

struct ImageProxy : Image {
    explicit ImageProxy(std::string file) : file_(std::move(file)) {}
    void display() const override {
        if (!real_) real_ = std::make_unique<RealImage>(file_);  // lazy load
        real_->display();
    }
private:
    std::string               file_;
    mutable std::unique_ptr<RealImage> real_;
};

// ── 8. Facade ─────────────────────────────────────────────────────────────────
// Provides a simple interface to a complex subsystem.
class VideoConverter {
public:
    std::string convert(const std::string& file, const std::string& format) {
        std::cout << "  Decoding " << file << "...\n";
        std::cout << "  Encoding to " << format << "...\n";
        return "output." + format;
    }
};
class AudioMixer {
public:
    void normalize(const std::string& file) {
        std::cout << "  Normalizing audio for " << file << "\n";
    }
};

class MediaFacade {   // Simple interface hiding VideoConverter + AudioMixer
public:
    std::string process(const std::string& input, const std::string& fmt) {
        auto out = vc_.convert(input, fmt);
        am_.normalize(out);
        return out;
    }
private:
    VideoConverter vc_;
    AudioMixer     am_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// BEHAVIORAL PATTERNS
// ═══════════════════════════════════════════════════════════════════════════════

// ── 9. Observer ───────────────────────────────────────────────────────────────
// Objects (observers) register to be notified of changes in another (subject).
class EventEmitter {
public:
    using Handler = std::function<void(const std::string&)>;

    void on(const std::string& event, Handler h) {
        listeners_[event].push_back(std::move(h));
    }
    void emit(const std::string& event, const std::string& data = "") {
        auto it = listeners_.find(event);
        if (it != listeners_.end())
            for (auto& h : it->second) h(data);
    }
private:
    std::unordered_map<std::string, std::vector<Handler>> listeners_;
};

// ── 10. Strategy ──────────────────────────────────────────────────────────────
// Define a family of algorithms, make them interchangeable at runtime.
struct SortStrategy {
    virtual void sort(std::vector<int>& v) const = 0;
    virtual ~SortStrategy() = default;
};

struct BubbleSort : SortStrategy {
    void sort(std::vector<int>& v) const override {
        std::cout << "  BubbleSort\n";
        for (size_t i = 0; i < v.size(); ++i)
            for (size_t j = 0; j+1 < v.size()-i; ++j)
                if (v[j] > v[j+1]) std::swap(v[j], v[j+1]);
    }
};

struct StdSort : SortStrategy {
    void sort(std::vector<int>& v) const override {
        std::cout << "  std::sort\n";
        std::sort(v.begin(), v.end());
    }
};

class Sorter {
public:
    explicit Sorter(std::unique_ptr<SortStrategy> s) : strategy_(std::move(s)) {}
    void set_strategy(std::unique_ptr<SortStrategy> s) { strategy_ = std::move(s); }
    void sort(std::vector<int>& v) const { strategy_->sort(v); }
private:
    std::unique_ptr<SortStrategy> strategy_;
};

// ── 11. Command ───────────────────────────────────────────────────────────────
// Encapsulates a request as an object — enables undo/redo, queuing.
struct Command {
    virtual void execute() = 0;
    virtual void undo()    = 0;
    virtual ~Command()     = default;
};

struct TextEditor {
    std::string text;
    void print() const { std::cout << "Editor: '" << text << "'\n"; }
};

struct InsertCommand : Command {
    TextEditor& editor;
    std::string inserted;
    size_t      pos;

    InsertCommand(TextEditor& e, size_t p, std::string s)
        : editor(e), inserted(std::move(s)), pos(p) {}

    void execute() override {
        editor.text.insert(pos, inserted);
    }
    void undo() override {
        editor.text.erase(pos, inserted.length());
    }
};

class CommandHistory {
public:
    void execute(std::unique_ptr<Command> cmd) {
        cmd->execute();
        history_.push(std::move(cmd));
    }
    void undo() {
        if (!history_.empty()) {
            history_.top()->undo();
            history_.pop();
        }
    }
private:
    std::stack<std::unique_ptr<Command>> history_;
};

// ── 12. State ─────────────────────────────────────────────────────────────────
// Object changes behavior based on its internal state.
struct TrafficLight;

struct LightState {
    virtual void handle(TrafficLight& light) = 0;
    virtual std::string name() const = 0;
    virtual ~LightState() = default;
};

struct TrafficLight {
    std::unique_ptr<LightState> state;
    void change();
    void status() const;
};

struct RedState : LightState {
    void handle(TrafficLight& light) override;
    std::string name() const override { return "RED"; }
};
struct GreenState : LightState {
    void handle(TrafficLight& light) override;
    std::string name() const override { return "GREEN"; }
};
struct YellowState : LightState {
    void handle(TrafficLight& light) override;
    std::string name() const override { return "YELLOW"; }
};

void RedState::handle(TrafficLight& light)    { light.state = std::make_unique<GreenState>(); }
void GreenState::handle(TrafficLight& light)  { light.state = std::make_unique<YellowState>(); }
void YellowState::handle(TrafficLight& light) { light.state = std::make_unique<RedState>(); }

void TrafficLight::change() { state->handle(*this); }
void TrafficLight::status() const { std::cout << "Light: " << state->name() << "\n"; }

// ── 13. Dependency Injection ──────────────────────────────────────────────────
// Provide dependencies from outside rather than creating them inside.
// Makes code testable and loosely coupled.
struct IDatabase {
    virtual std::string query(const std::string& sql) const = 0;
    virtual ~IDatabase() = default;
};

struct RealDatabase : IDatabase {
    std::string query(const std::string& sql) const override {
        return "real_result for: " + sql;
    }
};

struct MockDatabase : IDatabase {   // for unit testing
    std::string query(const std::string& sql) const override {
        return "mock_result for: " + sql;
    }
};

class UserService {
public:
    // Dependency injected via constructor — not created internally
    explicit UserService(IDatabase& db) : db_(db) {}

    std::string get_user(int id) const {
        return db_.query("SELECT * FROM users WHERE id=" + std::to_string(id));
    }
private:
    IDatabase& db_;
};

// ───────────────────────────────────────────────────────────────────────────────
// main — demonstrate all patterns
// ───────────────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══ Topic 16: Design Patterns ══╗\n";

    // Singleton
    std::cout << "\n=== Singleton ===\n";
    Logger::instance().log("Application started");
    Logger::instance().log("Another log entry");

    // Factory
    std::cout << "\n=== Factory ===\n";
    for (const auto& kind : {"dog","cat","bird"}) {
        auto a = make_animal(kind);
        if (a) { std::cout << a->type() << ": "; a->speak(); }
    }

    // Abstract Factory
    std::cout << "\n=== Abstract Factory ===\n";
    auto factory = std::make_unique<MacFactory>();
    factory->create_button()->render();
    factory->create_checkbox()->render();

    // Builder
    std::cout << "\n=== Builder ===\n";
    auto pizza = PizzaBuilder()
        .crust("thin")
        .sauce("tomato")
        .topping("cheese")
        .topping("pepperoni")
        .topping("mushrooms")
        .build();
    pizza.print();

    // Prototype
    std::cout << "\n=== Prototype ===\n";
    Config orig{"db.example.com", 5432, true};
    auto cloned = orig.clone();
    cloned->port = 5433;
    orig.print();
    cloned->print();

    // Adapter
    std::cout << "\n=== Adapter ===\n";
    LegacyPrinter lp;
    PrinterAdapter adapted(lp);
    adapted.print("Hello via Adapter");

    // Decorator
    std::cout << "\n=== Decorator ===\n";
    auto text = std::make_unique<PlainText>("Hello");
    auto bold = std::make_unique<BoldDecorator>(std::move(text));
    auto boldItalic = std::make_unique<ItalicDecorator>(std::move(bold));
    std::cout << boldItalic->render() << "\n";

    // Proxy
    std::cout << "\n=== Proxy ===\n";
    ImageProxy img("photo.jpg");
    std::cout << "First display:\n";  img.display();
    std::cout << "Second display:\n"; img.display();   // no reload

    // Facade
    std::cout << "\n=== Facade ===\n";
    MediaFacade media;
    auto out = media.process("movie.avi", "mp4");
    std::cout << "Output: " << out << "\n";

    // Observer
    std::cout << "\n=== Observer ===\n";
    EventEmitter events;
    events.on("login",  [](const std::string& u){ std::cout << "Notify: " << u << " logged in\n"; });
    events.on("login",  [](const std::string& u){ std::cout << "Audit:  " << u << " login event\n"; });
    events.on("logout", [](const std::string& u){ std::cout << "Notify: " << u << " logged out\n"; });
    events.emit("login",  "Alice");
    events.emit("logout", "Alice");

    // Strategy
    std::cout << "\n=== Strategy ===\n";
    std::vector<int> data = {5,3,1,4,2};
    Sorter sorter(std::make_unique<BubbleSort>());
    sorter.sort(data);
    for (auto x : data) std::cout << x << " ";  std::cout << "\n";

    data = {5,3,1,4,2};
    sorter.set_strategy(std::make_unique<StdSort>());
    sorter.sort(data);
    for (auto x : data) std::cout << x << " ";  std::cout << "\n";

    // Command
    std::cout << "\n=== Command ===\n";
    TextEditor editor;
    CommandHistory history;
    history.execute(std::make_unique<InsertCommand>(editor, 0, "Hello"));
    editor.print();
    history.execute(std::make_unique<InsertCommand>(editor, 5, " World"));
    editor.print();
    history.undo();
    editor.print();
    history.undo();
    editor.print();

    // State
    std::cout << "\n=== State ===\n";
    TrafficLight light{std::make_unique<RedState>()};
    for (int i = 0; i < 6; ++i) {
        light.status();
        light.change();
    }

    // Dependency Injection
    std::cout << "\n=== Dependency Injection ===\n";
    RealDatabase real_db;
    MockDatabase mock_db;
    UserService prod_service(real_db);
    UserService test_service(mock_db);
    std::cout << prod_service.get_user(1) << "\n";
    std::cout << test_service.get_user(1) << "\n";

    std::cout << "\n✓ Topic 16 complete.\n";
}
