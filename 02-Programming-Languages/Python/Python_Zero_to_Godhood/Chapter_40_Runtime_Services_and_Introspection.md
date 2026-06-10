# Runtime Services and Introspection


Introspection is the ability of a program to examine its own state and structure at runtime. Python's dynamic nature makes it one of the most introspective languages, providing deep access to its own interpreter state and the structure of its code.

### 40.1 `sys`: The Interpreter Interface

The `sys` module provides variables and functions that interact strongly with the interpreter.

#### 1. Runtime Environment
*   `sys.argv`: The command-line arguments passed to the script.
*   `sys.path`: The list of strings that specifies the search path for modules.
*   `sys.modules`: The dictionary that maps module names to modules which have already been loaded.

#### 2. Resource Management
*   `sys.getrefcount(obj)`: Returns the reference count of the object (always one higher than expected because of the argument to `getrefcount`).
*   `sys.getsizeof(obj)`: Returns the size of an object in bytes (calls the `tp_basicsize` and `tp_itemsize` C slots).

#### 3. Low-Level Hooks
*   `sys.settrace(func)`: Sets the system's trace function, allowing you to implement debuggers and code coverage tools.
*   `sys.setprofile(func)`: Sets the system's profile function for performance analysis.

### 40.2 `inspect`: Deep Object Analysis

The `inspect` module provides functions for learning about live objects.

#### 1. Type Checking and Members
*   `inspect.getmembers(obj)`: Returns all members of an object in a list of `(name, value)` pairs.
*   `inspect.isfunction()`, `inspect.isclass()`: Reliable ways to check object types.

#### 2. Retrieving Source Code
`inspect.getsource(obj)` retrieves the source code of a function or class by looking up the filename in the object's code object and reading from the disk.

#### 3. Signatures and Parameters
`inspect.signature(func)` returns a `Signature` object. This is more than just a list of names; it includes default values, type hints, and the "kind" of parameter (positional-only, keyword-only, etc.).

#### 4. The Stack Frame
`inspect.currentframe()` and `inspect.stack()` allow you to walk the execution stack. You can see which function called the current one, access its local variables, and even modify them (though this is extremely dangerous and rarely recommended).

### 40.3 `warnings`: Managing Runtime Diagnostics

The `warnings` module is used to issue alerts about non-fatal issues (e.g., deprecated features).
*   **Filters**: You can control whether warnings are ignored, printed, or turned into exceptions using `warnings.filterwarnings()` or the `-W` command-line switch.
*   **Context**: Warnings include the line of code that triggered them, making them more useful than simple `print()` statements for developers.

### 40.4 `ast`: Programmatic Source Analysis

The `ast` module (briefly touched upon in Chapter 31) allows you to manipulate Python code as a tree structure.
*   **`ast.NodeVisitor`**: A class that you subclass to traverse the tree and perform actions at specific nodes (e.g., finding all function calls).
*   **`ast.NodeTransformer`**: A subclass that allows you to modify the tree, effectively performing "source-to-source" compilation or code instrumentation.

---


---