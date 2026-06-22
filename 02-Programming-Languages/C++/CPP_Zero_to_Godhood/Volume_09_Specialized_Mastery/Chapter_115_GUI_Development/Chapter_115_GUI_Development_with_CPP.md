# Chapter 115: GUI Development with C++

Graphical user interfaces sit at the opposite end of C++'s spectrum from lock-free queues — they are event-driven, stateful, and human-facing — but they are still where much of the world's professional C++ lives: CAD tools, DAWs, IDEs, game editors, and trading terminals. The central design question of any GUI is *how UI state relates to the rendered pixels*, and the two answers — retained mode and immediate mode — lead to fundamentally different architectures. This chapter covers both through their flagship C++ frameworks, Qt and Dear ImGui, and the threading discipline GUIs demand.

## Chapter Roadmap

- 115.1 The Two GUI Paradigms
- 115.2 Retained Mode: Qt and Signals/Slots
- 115.3 Immediate Mode: Dear ImGui
- 115.4 The UI Thread Rule
- 115.5 Choosing a Paradigm

---

## 115.1 The Two GUI Paradigms

Every GUI framework answers one question: where does the UI state live, and how does it become pixels?

- **Retained mode** keeps a persistent object tree of widgets (buttons, panels). You build the tree once; the framework retains it, renders it, and you mutate it in response to events. The framework owns the state.
- **Immediate mode** has no persistent widget tree. Every frame, you *call* functions that describe the UI ("draw a button here") and the framework draws it immediately; the UI is a *function of your application state*, re-derived every frame.

> **Why this matters.** This choice determines the entire architecture. Retained mode (Qt, GTK, the web DOM) suits complex, mostly-static UIs with many widgets and accessibility requirements — the framework manages layout, state, and redraw efficiency, but you must keep the widget tree *synchronised* with your data (the source of a thousand "the UI didn't update" bugs). Immediate mode (Dear ImGui) suits dynamic, data-driven UIs — debug overlays, game editors, tools — where the UI *is* a direct view of state, eliminating synchronisation bugs at the cost of redrawing every frame. The paradigms are not better/worse; they fit different problems, and knowing which you are in explains the framework's idioms.

---

## 115.2 Retained Mode: Qt and Signals/Slots

**Qt** is the dominant cross-platform retained-mode C++ framework. Its signature feature is the **signal/slot** mechanism — a type-safe observer pattern (Chapter 107) for connecting events to handlers — implemented via the **Meta-Object Compiler (MOC)**, a code generator that processes Qt's `Q_OBJECT` macro before the normal compile.

```cpp
// Min standard: C++11 + Qt (non-portable: requires Qt + MOC). A window with a connected button.
class MainWindow : public QMainWindow {
    Q_OBJECT                                  // MOC processes this class for signals/slots
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        auto* button = new QPushButton("Click me", this);
        connect(button, &QPushButton::clicked,    // signal
                this,   &MainWindow::handleButton);  // slot — type-checked connection
    }
public slots:
    void handleButton() { /* respond to the click */ }
};
```
*Listing 115.1 — Qt signals/slots: a type-safe event connection. `Q_OBJECT` is processed by the MOC. Non-portable.*

> **Why this matters / cost model.** Signals/slots are Qt's answer to the Observer pattern's lifetime hazard (Chapter 107): a connection is automatically severed when either the sender or receiver `QObject` is destroyed, eliminating the dangling-pointer bug that plagues naive observer implementations. The cost is the **MOC**: Qt extends C++ with a pre-compilation code-generation step (because C++ historically lacked reflection — Chapter 75), which is why Qt projects need the MOC in their build (Chapter 110) and why `Q_OBJECT` classes have special rules. Qt also brings its own object model (parent-owned `QObject` trees with automatic cleanup, replacing manual memory management for widgets) and a rich widget/layout/accessibility system — the reason it powers serious desktop applications. The trade-off is that you adopt Qt's whole paradigm (its memory model, its build step, its types like `QString`), not just a widget library.

---

## 115.3 Immediate Mode: Dear ImGui

**Dear ImGui** is the leading immediate-mode C++ library, ubiquitous in game engines and developer tools. There is no widget tree and no MOC — every frame, you call functions that both *describe and draw* the UI, reading and writing your application's state directly.

```cpp
// Min standard: C++11 + Dear ImGui (non-portable). UI re-described every frame.
void render_debug_panel(float bg_color[3]) {
    ImGui::Begin("Debug Tools");
    ImGui::ColorEdit3("Background", bg_color);     // reads & writes bg_color directly
    if (ImGui::Button("Reset")) {                   // returns true the frame it's clicked
        bg_color[0] = bg_color[1] = bg_color[2] = 0.0f;
    }
    ImGui::End();
}
// Called every frame inside the render loop.
```
*Listing 115.2 — Dear ImGui: the UI is a function of state, re-described each frame. Non-portable.*

> **Why this matters / cost model.** Immediate mode's defining advantage is the *elimination of synchronisation bugs*: because the UI is recomputed from your state every frame, there is no widget tree to fall out of sync — a button's enabled-state is just `if (state.ready)`, not a separate `button->setEnabled()` call you might forget. This makes it ideal for rapidly-changing, data-driven UIs (game-engine inspectors, profiler overlays, debug tools) and trivial to add to an existing render loop. The cost is literal: it *re-renders the entire UI every frame* (typically 60+ times/sec), so it is appropriate where you are *already* rendering continuously (a game, a visualizer) but wasteful for a mostly-static document UI that a retained-mode framework would redraw only on change. It also lacks the accessibility, complex layout, and native look-and-feel of retained-mode toolkits.

---

## 115.4 The UI Thread Rule

Nearly every GUI framework imposes a hard rule: **all UI operations must happen on a single thread** (the "UI thread" or "main thread"). Background work runs on other threads and must *marshal* results back to the UI thread to update widgets.

```cpp
// Min standard: C++11. The pattern: do work off the UI thread, post the result back.
// Worker thread: compute result ...
// Then marshal to the UI thread (Qt: queued signal/slot connection; others: a posted task):
//   QMetaObject::invokeMethod(widget, [result]{ widget->setText(result); }, Qt::QueuedConnection);
```
*Listing 115.3 — Background work marshals its result to the UI thread; widgets are never touched off-thread.*

> **Why this matters.** The single-UI-thread rule exists because GUI toolkits' state is not thread-safe (making every widget operation locked would cripple performance), and the OS's event/windowing system itself often requires single-threaded access. Violating it — touching a widget from a worker thread — is a data race (Chapter 76), producing intermittent crashes and corruption that are agonising to debug. The discipline is the hot/cold split (Chapter 106) in GUI form: keep the UI thread responsive by doing *no* blocking or heavy work on it (a slow operation on the UI thread freezes the entire interface — the "spinning beachball"), run that work on background threads or coroutines, and marshal only the *results* back via the framework's thread-safe mechanism (Qt's queued connections, a posted task, an async callback). A responsive GUI is one whose UI thread never blocks.

---

## 115.5 Choosing a Paradigm

| Aspect | Retained mode (Qt) | Immediate mode (ImGui) |
|---|---|---|
| State ownership | Framework (widget tree) | Your application |
| Synchronisation bugs | Possible (tree vs data) | Eliminated (UI = f(state)) |
| Redraw cost | Only on change | Every frame |
| Best for | Complex static UIs, accessibility | Dynamic tools, game editors, overlays |
| Build complexity | MOC code-gen step | Header-only, drop-in |
| Threading | Single UI thread | Single UI thread |

> **The discipline.** GUI development is governed by two decisions: the paradigm (retained vs immediate — chosen by whether the UI is complex-and-static or dynamic-and-data-driven) and the universal threading rule (all UI on one thread, heavy work off it, results marshalled back). Both reduce to managing the relationship between *state* and *pixels*: retained mode asks you to keep a widget tree synchronised with your data, immediate mode re-derives the UI from your data every frame. Whichever you choose, never block or race the UI thread — the same hot-path discipline that keeps a server's latency low keeps a GUI responsive. The next chapters move to the compute-heavy domains: scientific computing and the GPU.
