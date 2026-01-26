/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: SOLID Principles in C++
 * Description: A complete example demonstrating all 5 SOLID principles.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <string>

// ==========================================
// 1. Single Responsibility Principle (SRP)
// A class should have only one reason to change.
// ==========================================

struct Journal {
    std::string title;
    std::vector<std::string> entries;

    Journal(const std::string& title) : title(title) {}

    void add_entry(const std::string& entry) {
        static int count = 1;
        entries.push_back(std::to_string(count++) + ": " + entry);
    }
    
    // BAD: Saving is a separate concern (Persistence)
    // void save(const std::string& filename) { ... }
};

class PersistenceManager {
public:
    static void save(const Journal& j, const std::string& filename) {
        std::cout << "Saving journal '" << j.title << "' to " << filename << "\n";
    }
};

// ==========================================
// 2. Open/Closed Principle (OCP)
// Open for extension, but closed for modification.
// ==========================================

enum class Color { Red, Green, Blue };
enum class Size { Small, Medium, Large };

struct Product {
    std::string name;
    Color color;
    Size size;
};

// Specification Pattern (for flexible filtering)
template <typename T>
struct Specification {
    virtual bool is_satisfied(const T* item) const = 0;
};

// Concrete Specification
struct ColorSpecification : Specification<Product> {
    Color color;
    ColorSpecification(Color c) : color(c) {}
    bool is_satisfied(const Product* item) const override {
        return item->color == color;
    }
};

// Filter Interface (Open for extension)
template <typename T>
struct Filter {
    virtual std::vector<T*> filter(std::vector<T*>& items, Specification<T>& spec) = 0;
};

struct BetterFilter : Filter<Product> {
    std::vector<Product*> filter(std::vector<Product*>& items, Specification<Product>& spec) override {
        std::vector<Product*> result;
        for (auto& p : items)
            if (spec.is_satisfied(p))
                result.push_back(p);
        return result;
    }
};

// ==========================================
// 3. Liskov Substitution Principle (LSP)
// Subtypes must be substitutable for their base types.
// ==========================================

class Rectangle {
protected:
    int width, height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    virtual int get_width() const { return width; }
    virtual int get_height() const { return height; }
    virtual void set_width(int w) { width = w; }
    virtual void set_height(int h) { height = h; }
    int area() const { return width * height; }
};

// BAD: Square inheriting from Rectangle breaks LSP if setters modify both dimensions unexpectedly.
// Better to have a separate Shape interface or not inherit if behavior differs significantly.

// ==========================================
// 4. Interface Segregation Principle (ISP)
// Clients should not be forced to depend on interfaces they do not use.
// ==========================================

struct IPrinter {
    virtual void print(const std::string& content) = 0;
};

struct IScanner {
    virtual void scan(const std::string& content) = 0;
};

// Machine implements only what it needs
struct Printer : IPrinter {
    void print(const std::string& content) override {
        std::cout << "Printing: " << content << "\n";
    }
};

struct MultiFunctionMachine : IPrinter, IScanner {
    void print(const std::string& content) override { /* ... */ }
    void scan(const std::string& content) override { /* ... */ }
};

// ==========================================
// 5. Dependency Inversion Principle (DIP)
// High-level modules should not depend on low-level modules. Both should depend on abstractions.
// ==========================================

struct ILogger {
    virtual void log(const std::string& message) = 0;
    virtual ~ILogger() = default;
};

struct ConsoleLogger : ILogger {
    void log(const std::string& message) override {
        std::cout << "LOG: " << message << "\n";
    }
};

class ReportingService { // High-level module
    std::shared_ptr<ILogger> logger; // Depends on abstraction (ILogger), not detail (ConsoleLogger)
public:
    ReportingService(std::shared_ptr<ILogger> logger) : logger(logger) {}
    void report() {
        logger->log("Report generated.");
    }
};

int main() {
    // SRP
    Journal j("My Diary");
    j.add_entry("I learned SOLID today.");
    PersistenceManager::save(j, "diary.txt");

    // OCP
    Product apple{"Apple", Color::Green, Size::Small};
    Product tree{"Tree", Color::Green, Size::Large};
    Product house{"House", Color::Blue, Size::Large};
    std::vector<Product*> all{&apple, &tree, &house};
    
    BetterFilter bf;
    ColorSpecification green(Color::Green);
    auto green_things = bf.filter(all, green);
    std::cout << "Found " << green_things.size() << " green things.\n";

    // DIP
    auto logger = std::make_shared<ConsoleLogger>();
    ReportingService service(logger);
    service.report();

    return 0;
}
