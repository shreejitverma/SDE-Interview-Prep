# Appendix L: Exhaustive Python Built-in Functions Reference


This appendix provides a complete list of Python 3.13 built-in functions, categorized by their primary use case.

### L.1 Object Creation and Conversion
*   **`bool(x)`**: Convert to boolean using truth testing.
*   **`bytearray([source[, encoding[, errors]]])`**: Return a mutable byte array.
*   **`bytes([source[, encoding[, errors]]])`**: Return an immutable bytes object.
*   **`complex([real[, imag]])`**: Create a complex number.
*   **`dict(**kwargs)`**: Create a new dictionary.
*   **`float(x)`**: Convert to floating-point.
*   **`frozenset([iterable])`**: Create an immutable set.
*   **`int(x[, base])`**: Convert to integer.
*   **`list([iterable])`**: Create a list.
*   **`set([iterable])`**: Create a set.
*   **`str(object='')`**: Convert to string.
*   **`tuple([iterable])`**: Create a tuple.

### L.2 Mathematical Operations
*   **`abs(x)`**: Absolute value.
*   **`divmod(a, b)`**: Return `(a // b, a % b)`.
*   **`max(iterable[, key])`**: Return the largest item.
*   **`min(iterable[, key])`**: Return the smallest item.
*   **`pow(base, exp[, mod])`**: Return `base**exp % mod`.
*   **`round(number[, ndigits])`**: Round to nearest integer or precision.
*   **`sum(iterable[, start])`**: Sum of all items.

### L.3 Sequence and Iteration
*   **`all(iterable)`**: True if all elements are true.
*   **`any(iterable)`**: True if any element is true.
*   **`enumerate(iterable, start=0)`**: Return an enumerate object (index, value).
*   **`filter(function, iterable)`**: Construct an iterator from elements where function is true.
*   **`iter(object[, sentinel])`**: Return an iterator object.
*   **`len(s)`**: Length of an object.
*   **`map(function, iterable, ...)`**: Apply function to every item of iterable.
*   **`next(iterator[, default])`**: Retrieve the next item from an iterator.
*   **`range(stop)`**: Create an arithmetic progression.
*   **`reversed(seq)`**: Return a reverse iterator.
*   **`slice(stop)`**: Create a slice object.
*   **`sorted(iterable[, key][, reverse])`**: Return a new sorted list.
*   **`zip(*iterables)`**: Aggregate elements from each of the iterables.

### L.4 Reflection and Introspection
*   **`callable(object)`**: True if object appears callable.
*   **`dir([object])`**: List of valid attributes for the object.
*   **`getattr(object, name[, default])`**: Get a named attribute.
*   **`hasattr(object, name)`**: True if object has the named attribute.
*   **`id(object)`**: Unique identity of an object (memory address in CPython).
*   **`isinstance(object, classinfo)`**: Check if object is an instance of a class.
*   **`issubclass(class, classinfo)`**: Check if a class is a subclass of another.
*   **`locals()`**: Update and return a dictionary representing the current local symbol table.
*   **`globals()`**: Return the dictionary representing the current global symbol table.
*   **`repr(object)`**: Return a string containing a printable representation of an object.
*   **`setattr(object, name, value)`**: Set a named attribute.
*   **`type(object)`**: Return the type of an object.
*   **`vars([object])`**: Return the `__dict__` attribute of an object.

---
