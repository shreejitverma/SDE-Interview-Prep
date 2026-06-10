# Matplotlib: The Artist Layer and Backend Architecture


Matplotlib uses a three-layer architecture:
1.  **Backend Layer**: Handles the actual rendering to a file (PNG, PDF) or screen (Qt, Tk).
2.  **Artist Layer**: Manages the hierarchy of objects (Figures, Axes, Lines).
3.  **Scripting Layer (`pyplot`)**: Provides the familiar state-machine interface.

---

## Phase XX: Web Framework Architectures

# Chapter 89: WSGI vs. ASGI: The Evolution of Web Interfaces

### 89.1 WSGI (Web Server Gateway Interface)
Defined in PEP 3333, WSGI is synchronous. The server calls a function for every request and waits for the response.
*   **Servers**: Gunicorn, uWSGI.

### 89.2 ASGI (Asynchronous Server Gateway Interface)
ASGI (PEP 3112) is the asynchronous successor, supporting WebSockets and long-lived connections.
*   **Servers**: Uvicorn, Daphne.

---
