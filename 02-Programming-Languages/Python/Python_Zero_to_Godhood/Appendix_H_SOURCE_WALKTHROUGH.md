# Appendix H: CPython Source Code Walkthrough (Core Objects)

This appendix provides a line-by-line analysis of the most critical C functions in the CPython source code, allowing for an absolute understanding of how the core data structures operate.

### H.1 `Objects/listobject.c`: `list_resize`

When you append to a list and it exceeds its current capacity, CPython resizes the underlying array using an over-allocation strategy.

```c
static int
list_resize(PyListObject *self, Py_ssize_t newsize)
{
    PyObject **items;
    size_t cur_allocated = (size_t)self->allocated;
    size_t allocated;

    if (cur_allocated >= (size_t)newsize && newsize >= (cur_allocated >> 1)) {
        assert(self->ob_item != NULL || newsize == 0);
        Py_SET_SIZE(self, newsize);
        return 0;
    }

    /* This over-allocation pattern is intended to give
       amortized O(1) performance for series of appends. */
    allocated = ((size_t)newsize + (newsize >> 3) + 6) & ~(size_t)3;
    if (newsize == 0)
        allocated = 0;

    items = (PyObject **)PyMem_Realloc(self->ob_item, allocated * sizeof(PyObject *));
    if (items == NULL) {
        PyErr_NoMemory();
        return -1;
    }
    self->ob_item = items;
    self->allocated = (Py_ssize_t)allocated;
    Py_SET_SIZE(self, newsize);
    return 0;
}
```
*   **The Over-allocation Formula**: `(newsize + (newsize >> 3) + 6) & ~3`. This ensures the list grows by about 12.5% each time, plus a small constant, and remains aligned to a 4-item boundary.

### H.2 `Objects/dictobject.c`: `lookdict_unicode`

This is the highly optimized lookup function for dictionaries where all keys are Unicode strings (the most common case).

```c
static Py_ssize_t
lookdict_unicode(PyDictObject *mp, PyObject *key, Py_hash_t hash)
{
    PyDictUnicodeEntry *ep0 = DK_UNICODE_ENTRIES(mp->ma_keys);
    size_t mask = DK_MASK(mp->ma_keys);
    size_t i = (size_t)hash & mask;
    PyDictUnicodeEntry *ep = &ep0[i];

    if (ep->me_key == NULL) return i;
    if (ep->me_key == key) return i;

    // ... Collision handling (linear probing with perturbation) ...
    for (size_t perturb = (size_t)hash; ; perturb >>= PERTURB_SHIFT) {
        i = (i << 2) + i + perturb + 1;
        ep = &ep0[i & mask];
        if (ep->me_key == NULL || ep->me_key == key) return i & mask;
    }
}
```
*   **Optimization**: Note the use of `(i << 2) + i` (which is `5*i`). This is a fast way to perform the linear probing calculation without a slow multiplication instruction.

---
