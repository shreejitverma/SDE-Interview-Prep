# Appendix P: CPython Memory Allocator Diagrams


This appendix provides visual descriptions (ASCII-art) of the memory pools and blocks used by `PyMalloc`.

### P.1 The Arena Structure
```text
+-----------------------------------------------------------+
|                          ARENA (256 KB)                   |
+-----------+-----------+-----------+-----------+-----------+
| POOL (4KB)| POOL (4KB)| POOL (4KB)| POOL (4KB)| POOL (4KB)|
+-----------+-----------+-----------+-----------+-----------+
| BLOCK(8B) | BLOCK(8B) | ...       | BLOCK(8B) | BLOCK(8B) |
+-----------+-----------+-----------+-----------+-----------+
```

### P.2 The Small Object Allocator Workflow
1.  **Request**: Python requests 32 bytes for a small string.
2.  **Size Class**: `PyMalloc` identifies this as Size Class 3.
3.  **Pool Check**: It checks the `usedpools` array for Size Class 3.
4.  **Block Return**: It returns a pointer to the next free block in the pool.
5.  **Alignment**: Blocks are always 8-byte aligned to ensure hardware efficiency (Chapter 70).

---
