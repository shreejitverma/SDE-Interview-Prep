# Web Browser and URL Automation


Python can control the user's web browser for simple automation tasks.

### 96.1 The `webbrowser` Module
*   **`open(url)`**: Opens the URL in the system's default browser.
*   **`open_new_tab(url)`**: Specifically requests a new tab.

### 96.2 URL Parsing and Query Strings
Integrating with `urllib.parse` (Chapter 54) to dynamically construct URLs with complex query parameters.

---

## Phase XXIII: Development Tooling and Maintenance

# Chapter 97: Comprehensive Logging Architectures

### 97.1 The Hierarchy of Loggers
Logging in Python uses a tree-based hierarchy.
*   **Propagation**: Child loggers pass messages up to their parents unless `propagate` is False.
*   **Handlers**: Direct the log messages to different destinations (Console, File, Network, Email).

### 97.2 The `logging.config` Dictionary
The most robust way to configure logging is via a dictionary (often loaded from JSON or YAML), allowing for a clean separation between code and configuration.

---
