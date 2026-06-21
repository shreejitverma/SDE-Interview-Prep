# GUI DEVELOPMENT WITH C


# GUI DEVELOPMENT WITH C++


Building desktop applications and tools.

### 22.1 Qt Framework (Retained Mode)
Qt uses a unique Signal/Slot mechanism (via MOC - Meta-Object Compiler).

```cpp
// MainWindow.h
class MainWindow : public QMainWindow {
    Q_OBJECT // Macro for MOC
public:
    MainWindow(QWidget *parent = nullptr);
public slots:
    void handleButton(); // Slot
};

// MainWindow.cpp
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QPushButton *button = new QPushButton("Click me", this);
    connect(button, &QPushButton::clicked, this, &MainWindow::handleButton);
}
```

### 22.2 Dear ImGui (Immediate Mode)
Ideal for game engines and internal tools. Re-renders UI every frame.

```cpp
// Main Loop
void Render() {
    ImGui::Begin("Debug Tools");
    static float col[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::ColorEdit3("Background Color", col);
    if (ImGui::Button("Reset")) {
        col[0] = col[1] = col[2] = 0.0f;
    }
    ImGui::End();
}
```

---


---
