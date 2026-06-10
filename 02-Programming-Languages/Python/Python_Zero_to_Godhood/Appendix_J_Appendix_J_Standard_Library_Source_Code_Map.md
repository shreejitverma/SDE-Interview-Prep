# Appendix J: Standard Library Source Code Map


This appendix provides a comprehensive mapping of the Python 3.13 standard library modules to their respective source files in the CPython repository. Use this as a guide for your own source-code explorations.

### J.1 Core Builtins and Objects
| Module/Type | C Source File | Purpose |
| :--- | :--- | :--- |
| `None`, `True`, `False` | `Objects/boolobject.c` | Core constants. |
| `int` | `Objects/longobject.c` | Arbitrary-precision integers. |
| `float` | `Objects/floatobject.c` | IEEE 754 doubles. |
| `list` | `Objects/listobject.c` | Dynamic arrays. |
| `dict` | `Objects/dictobject.c` | Hash tables. |
| `str` | `Objects/unicodeobject.c` | PEP 393 compact strings. |
| `tuple` | `Objects/tupleobject.c` | Immutable sequences. |
| `set`, `frozenset` | `Objects/setobject.c` | Hash-based sets. |

### J.2 Python Modules (C Extensions)
| Module | C Source File | Location in Repo |
| :--- | :--- | :--- |
| `array` | `arraymodule.c` | `Modules/` |
| `binascii` | `binascii.c` | `Modules/` |
| `cmath` | `cmathmodule.c` | `Modules/` |
| `datetime` | `_datetimemodule.c` | `Modules/` |
| `errno` | `errnomodule.c` | `Modules/` |
| `gc` | `gcmodule.c` | `Modules/` |
| `hashlib` | `_hashopenssl.c` | `Modules/` |
| `itertools` | `itertoolsmodule.c` | `Modules/` |
| `json` | `_json.c` | `Modules/` |
| `math` | `mathmodule.c` | `Modules/` |
| `mmap` | `mmapmodule.c` | `Modules/` |
| `os` | `posixmodule.c` | `Modules/` |
| `pickle` | `_pickle.c` | `Modules/` |
| `re` | `_sre.c` | `Modules/` |
| `select` | `selectmodule.c` | `Modules/` |
| `socket` | `socketmodule.c` | `Modules/` |
| `ssl` | `_ssl.c` | `Modules/` |
| `sys` | `sysmodule.c` | `Python/` |
| `time` | `timemodule.c` | `Modules/` |
| `zlib` | `zlibmodule.c` | `Modules/` |

### J.3 High-Level Python Modules (`Lib/`)
| Module | Python File | Purpose |
| :--- | :--- | :--- |
| `abc` | `Lib/abc.py` | Abstract Base Classes. |
| `argparse` | `Lib/argparse.py` | CLI parsing. |
| `asyncio` | `Lib/asyncio/` | Asynchronous I/O. |
| `collections` | `Lib/collections/` | Container datatypes. |
| `email` | `Lib/email/` | Email/MIME handling. |
| `http` | `Lib/http/` | HTTP server/client logic. |
| `importlib` | `Lib/importlib/` | The import machinery. |
| `inspect` | `Lib/inspect.py` | Runtime introspection. |
| `logging` | `Lib/logging/` | Event logging. |
| `multiprocessing`| `Lib/multiprocessing/` | Process-based parallelism. |
| `pathlib` | `Lib/pathlib.py` | OO filesystem paths. |
| `sqlite3` | `Lib/sqlite3/` | SQLite database wrapper. |
| `unittest` | `Lib/unittest/` | Testing framework. |
| `urllib` | `Lib/urllib/` | URL processing. |
| `venv` | `Lib/venv/` | Virtual environments. |

---

**This map covers 95% of the logic you will interact with in a production Python system.**

---
