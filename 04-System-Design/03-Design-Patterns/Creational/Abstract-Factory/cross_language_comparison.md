# Abstract Factory Pattern: Cross-Language Comparison

**Goal:** Create families of related objects (e.g., UI Buttons for Mac vs Windows) without specifying their concrete classes.

## C++ (Static Typing, Pointers)
```cpp
struct Button { virtual void paint() = 0; };
struct WinButton : Button { void paint() override { cout << "WinButton"; } };
struct MacButton : Button { void paint() override { cout << "MacButton"; } };

struct GUIFactory {
    virtual std::unique_ptr<Button> createButton() = 0;
};

// Concrete Factory
struct WinFactory : GUIFactory {
    std::unique_ptr<Button> createButton() override { return std::make_unique<WinButton>(); }
};
```

## Python (Dynamic Typing)
Python classes are first-class citizens, so factories are simpler.
```python
class WinButton:
    def paint(self): print("WinButton")

class MacButton:
    def paint(self): print("MacButton")

class WinFactory:
    def create_button(self): return WinButton()

# Usage
factory = WinFactory()
btn = factory.create_button()
```

## Java (Strict OOP, Generics)
```java
interface Button { void paint(); }
class WinButton implements Button { 
    public void paint() { System.out.println("WinButton"); } 
}

interface GUIFactory {
    Button createButton();
}

class WinFactory implements GUIFactory {
    public Button createButton() { return new WinButton(); }
}
```
