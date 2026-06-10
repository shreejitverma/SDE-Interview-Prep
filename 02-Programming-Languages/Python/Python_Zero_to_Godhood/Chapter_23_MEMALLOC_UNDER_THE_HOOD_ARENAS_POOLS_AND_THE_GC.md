# MEMALLOC UNDER THE HOOD: ARENAS, POOLS, AND THE GC

*   **PyMalloc**: CPython's custom memory allocator for small objects ($\le 512$ bytes).
*   **Garbage Collection**: Uses generational cycle-detection (three generations) to identify and sweep reference-cycle memory leaks.
