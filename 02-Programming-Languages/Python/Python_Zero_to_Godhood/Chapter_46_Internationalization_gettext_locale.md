# Internationalization (`gettext`, `locale`)


Software that reaches the world must be adaptable to local languages, customs, and cultural conventions. Python's `gettext` and `locale` modules provide the infrastructure for I18N (Internationalization) and L10N (Localization).

### 57.1 `gettext`: The GNU Translation Standard

`gettext` is the industry standard for message translation.
*   **The `.mo` Compiled Format**: Python's `gettext` module reads compiled message catalogs (`.mo` files). These are binary hash tables designed for near $O(1)$ message lookup, ensuring that translating a string like `_("Hello")` doesn't slow down the UI.
*   **The Underscore `_()` Alias**: By convention, the translation function is aliased to `_`. The `gettext` module can install this globally in the `builtins` namespace, allowing every module in the application to use it without explicit imports.

### 57.2 `locale`: Interfacing with OS Cultural Context

The `locale` module is a thin wrapper around the C library `setlocale()` and associated functions.
*   **Categories**: `LC_TIME` (Date formatting), `LC_MONETARY` (Currency), `LC_NUMERIC` (Decimal separators), `LC_COLLATE` (Sorting order).
*   **The Global State Problem**: Locales are process-global in C. Changing the locale in one thread affects the entire process. **Godhood Warning**: Be extremely careful when using `locale` in multi-threaded web servers. Modern Python (3.7+) has introduced better ways to handle thread-local context, but the underlying C locale remains global.

---
