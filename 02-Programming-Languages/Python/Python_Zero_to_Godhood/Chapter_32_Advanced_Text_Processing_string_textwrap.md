# Advanced Text Processing (`string`, `textwrap`)


While `str` methods cover basic needs, the `string` and `textwrap` modules handle complex formatting and layout logic, often interacting with terminal dimensions and internationalization.

### 43.1 `string.Formatter`: The Engine of `.format()`

The `f-string` (Chapter 12) is the fastest, but `string.Formatter` is the most extensible.
*   **`parse(format_string)`**: This method returns an iterator of `(literal_text, field_name, format_spec, conversion)`.
*   **`get_value(key, args, kwargs)`**: This is the hook for custom lookup logic.
*   **Internals**: F-strings are compiled to specialized bytecode (`FORMAT_VALUE`), whereas `.format()` calls into the `string` module's C-accelerated formatting logic.

### 43.2 `textwrap`: Dynamic Layout Management

`textwrap` is essential for CLI tools that must adapt to varying terminal widths.
*   **`TextWrapper` Object**: Maintains state for `width`, `indent`, and `break_long_words`.
*   **`wrap()` vs. `fill()`**: `wrap` returns a list of strings; `fill` returns a single newline-joined string.
*   **Algorithms**: It uses a greedy algorithm to fit words into the specified width, handling edge cases like hyphenated words and double-width Unicode characters correctly.

---
