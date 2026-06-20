# Chapter 29: Associative Container Advances

> *C++17 quietly reworked the associative containers — `map`, `set`, `unordered_map`, `unordered_set`, and their `multi` variants — to fix two long-standing inefficiencies: moving an element between containers used to require a copy-out/erase/insert-in cycle that reallocated the node, and inserting "if not already present" forced you to construct the value even when it would be thrown away. Node handles, `try_emplace`, and `insert_or_assign` close both gaps, and the `emplace`/`emplace_back` family now returns a reference to the element it created.*

This chapter is authored to complete the C++17 coverage; it has no predecessor source material. The features here are small in surface area but eliminate real, measurable waste. **Node handles** let you splice a node out of one container and into another — or change a key — *without touching the heap*, because the node's allocated storage moves intact. **`try_emplace`** solves the "insert-or-do-nothing" problem without constructing a doomed temporary or, crucially, without moving from your arguments when the key already exists. **`insert_or_assign`** is the honest "upsert" that tells you whether it inserted or overwrote. And the **reference-returning `emplace`/`emplace_back`** removes a redundant `back()` call after every emplacement. Each is a focused fix for a pattern that previously cost an allocation, a construction, or a move you did not need.

---

## Table of Contents

- [29.1 Node Handles: Moving Nodes Without Reallocation](#291-node-handles-moving-nodes-without-reallocation)
- [29.2 `extract`: Removing a Node Without Destroying It](#292-extract-removing-a-node-without-destroying-it)
- [29.3 `insert(node_handle)`: Splicing a Node In](#293-insertnode_handle-splicing-a-node-in)
- [29.4 `merge`: Bulk Node Transfer Between Containers](#294-merge-bulk-node-transfer-between-containers)
- [29.5 Changing a Key Through a Node Handle](#295-changing-a-key-through-a-node-handle)
- [29.6 `try_emplace`: Insert Without Constructing on Failure](#296-try_emplace-insert-without-constructing-on-failure)
- [29.7 `insert_or_assign`: The Honest Upsert](#297-insert_or_assign-the-honest-upsert)
- [29.8 Reference-Returning `emplace` and `emplace_back`](#298-reference-returning-emplace-and-emplace_back)
- [29.9 Professional Insights](#299-professional-insights)

---

## 29.1 Node Handles: Moving Nodes Without Reallocation

Before C++17, the only way to move an element from one associative container to another was to **copy or move the value out, erase the source node, and insert a fresh node** in the destination — two node allocations and a deallocation for what is conceptually a pointer re-link. For a `map<std::string, ExpensiveValue>` this meant reallocating and re-moving the whole pair.

C++17 introduces the **node handle** (`container::node_type`): a movable, owning handle to a single detached node — the actual allocated tree/bucket node, removed from its container but not destroyed. A node handle lets you take a node *out* of one container and put it *into* another (or back into the same one) **without any allocation**: the node's storage, and the element living inside it, are never copied or moved — only the container's internal links change.

The operations that produce and consume node handles are:

| Operation | Effect |
|-----------|--------|
| `c.extract(key)` / `c.extract(iter)` | detach a node, returning its `node_type` handle |
| `c.insert(std::move(handle))` | splice a node handle into `c` |
| `c.merge(other)` | move all transferable nodes from `other` into `c` |

A node handle is move-only and owns its node: if it goes out of scope still holding a node, that node is destroyed and its memory freed. An empty handle (after a failed `extract` or after its node is inserted) is contextually convertible to `false`.

---

## 29.2 `extract`: Removing a Node Without Destroying It

`extract` removes an element from the container and hands you ownership of its node via a `node_type`. The element is **not** destroyed — it lives on inside the handle, and you can inspect or modify it before deciding where it goes.

```cpp
// Listing 29.1: extract detaches a node without destroying its element
#include <map>
#include <string>

std::map<int, std::string> m{{1, "one"}, {2, "two"}, {3, "three"}};

// Detach the node for key 2. No deallocation occurs; the node now lives in 'nh'.
auto nh = m.extract(2);          // m is now {1:"one", 3:"three"}

if (nh) {                         // the handle owns a node
    // For a map, the handle exposes mutable key() and mapped():
    std::string& v = nh.mapped(); // "two" — can be modified in place
}
// If 'nh' is destroyed here still holding the node, the element is destroyed then.
```

For a `map`/`unordered_map` the handle exposes `key()` (a **mutable** reference — see Section 29.5) and `mapped()`; for a `set`/`unordered_set` it exposes `value()`. Because the node is detached, you can change the key without the container's invariant being temporarily violated — there is no container to violate while the node is out.

---

## 29.3 `insert(node_handle)`: Splicing a Node In

Passing a node handle to `insert` **splices** the node into the container — no allocation, no element copy or move. This is the destination half of the transfer.

```cpp
// Listing 29.2: transferring a node between maps with zero reallocation
#include <map>
#include <string>

std::map<int, std::string> src{{1, "one"}, {2, "two"}};
std::map<int, std::string> dst{{3, "three"}};

// Move the node holding {2, "two"} from src into dst — the std::string "two"
// is never copied or moved; only the node's links change.
auto result = dst.insert(src.extract(2));

// dst is now {2:"two", 3:"three"}; src is {1:"one"}.
// result is an insert_return_type:
//   result.inserted  -> bool: did the splice succeed?
//   result.position  -> iterator to the (existing or new) element
//   result.node      -> the handle, NON-empty if insertion FAILED (key clash)
```

For the unique-key containers, `insert(node)` returns an `insert_return_type` aggregate with three members: `inserted` (whether it went in), `position` (an iterator), and `node` (the handle, returned **non-empty if the key already existed** so you keep ownership and can react). For `multi` containers the splice always succeeds and `insert` returns a plain iterator. The decisive property throughout: the element inside the node is never reconstructed, so even non-movable or expensive-to-move mapped types transfer for the cost of a few pointer writes.

---

## 29.4 `merge`: Bulk Node Transfer Between Containers

`merge` moves **every transferable node** from a source container into the destination, again splicing rather than copying. Nodes whose keys would collide with existing keys in the destination are left behind in the source (for unique-key containers), so `merge` is a non-destructive "take everything that fits."

```cpp
// Listing 29.3: merge splices all non-conflicting nodes at once
#include <map>
#include <string>

std::map<int, std::string> a{{1, "a1"}, {2, "a2"}};
std::map<int, std::string> b{{2, "b2"}, {3, "b3"}};

a.merge(b);
// Key 2 already exists in 'a', so b's node for 2 stays in b.
// a -> {1:"a1", 2:"a2", 3:"b3"};  b -> {2:"b2"}  (the conflicting node remains)
```

No element is copied or moved and no node is allocated — `merge` is the bulk form of `extract` + `insert(node)`. The source and destination must use compatible node types (same key/value/allocator), but they may differ in comparator/hash, which makes `merge` useful for, e.g., consolidating several differently-ordered maps into one. Surviving conflicting nodes remain in the source, so you can inspect or re-home them afterward.

---

## 29.5 Changing a Key Through a Node Handle

A `map`'s key is `const` while the element is *in* the container — you cannot reassign it, because doing so would break the tree ordering. But once a node is **extracted**, it is in no container, so the node handle exposes the key as a **mutable** reference. This makes "rename a key, keeping its value" an allocation-free operation for the first time.

```cpp
// Listing 29.4: re-keying an element without reallocation
#include <map>
#include <string>

std::map<int, std::string> m{{1, "one"}, {2, "two"}};

auto nh = m.extract(1);   // detach node for key 1
nh.key() = 10;            // mutate the key — legal only while extracted
m.insert(std::move(nh));  // re-insert under the new key

// m is now {2:"two", 10:"one"} — the std::string "one" was never copied or moved.
```

Before C++17 this required erasing the old entry and inserting a new one — destroying and reconstructing the value, with a deallocation and an allocation. The node-handle route mutates one key field and re-links one node. The same technique re-homes a `set` element via `value()` when the element's identity (its key) must change but its storage should not.

---

## 29.6 `try_emplace`: Insert Without Constructing on Failure

`emplace` and `insert` have a subtle flaw for the "add if absent" pattern on maps: to attempt the insert they may **construct the value (and move from your arguments)** *before* discovering the key already exists — at which point the work is wasted, and worse, your arguments may have been moved-from. `try_emplace` fixes both problems with a precise contract: **if the key already exists, it does nothing and does not touch your arguments.**

```cpp
// Listing 29.5: try_emplace does not construct or move-from on a key clash
#include <map>
#include <string>
#include <memory>

std::map<std::string, std::unique_ptr<Widget>> cache;

auto ptr = std::make_unique<Widget>();

// If "key" is absent: constructs the value from std::move(ptr) and inserts.
// If "key" is PRESENT: does nothing, and ptr is NOT moved-from — still valid!
auto [it, inserted] = cache.try_emplace("key", std::move(ptr));

if (!inserted) {
    // ptr is still usable here because try_emplace left it alone:
    use(std::move(ptr));
}
```

Two properties make `try_emplace` the right default for "insert if missing":

- It takes the **key and the value-constructor-arguments separately** (`try_emplace(key, args...)`), constructing the mapped value in place from `args...` only when an insertion actually happens.
- On a key clash it **leaves the arguments untouched**, so move-only types (`unique_ptr`, `future`, streams) passed as arguments survive a failed insert and remain usable.

Contrast `emplace`/`insert`, which may move from a `pair` you constructed even when nothing is inserted — a classic source of silently-emptied `unique_ptr`s.

---

## 29.7 `insert_or_assign`: The Honest Upsert

The textbook `m[key] = value` "upsert" has two flaws: it requires the mapped type to be **default-constructible** (because `operator[]` default-constructs before assigning), and it **silently hides** whether it created a new entry or overwrote an existing one. `insert_or_assign` does the same job correctly: it inserts when absent and assigns when present, works with non-default-constructible types, and **reports which happened**.

```cpp
// Listing 29.6: insert_or_assign reports insert vs overwrite
#include <map>
#include <string>

std::map<std::string, int> scores;

auto [it1, inserted1] = scores.insert_or_assign("alice", 10);
// inserted1 == true  — "alice" was newly added

auto [it2, inserted2] = scores.insert_or_assign("alice", 20);
// inserted2 == false — "alice" existed; its value was reassigned to 20
```

Unlike `try_emplace`, `insert_or_assign` *does* update the value when the key is present (that is its whole point), but it assigns directly to the existing mapped object rather than constructing a temporary. And unlike `operator[]`, it never default-constructs, so it works for mapped types that have no default constructor — and its `bool` return lets the caller distinguish a create from an update, which `operator[]` cannot.

Use the two together by intent: `try_emplace` for "insert only if missing, leave existing alone," `insert_or_assign` for "insert or overwrite, and tell me which."

---

## 29.8 Reference-Returning `emplace` and `emplace_back`

Before C++17, `vector::emplace_back` and the other sequence-container `emplace_back`/`emplace_front` returned `void`, so the universal idiom was to emplace and then call `back()` to use the just-created element — a redundant access. **C++17 changes `emplace_back` and `emplace_front` to return a reference to the element they construct.**

```cpp
// Listing 29.7: emplace_back returns the new element directly
#include <vector>
#include <string>

std::vector<std::string> v;

// Pre-C++17: emplace_back returned void, forcing a follow-up back():
//   v.emplace_back("hello");
//   std::string& s = v.back();

// C++17: the reference comes straight back:
std::string& s = v.emplace_back("hello");
s += " world";                 // operate on the new element immediately
```

The returned reference is to the newly constructed element, so you can configure it in place without the extra `back()`/`front()` lookup (and without the risk of indexing the wrong end). This is a small ergonomic win, but it removes a near-universal two-line pattern and makes "create and then immediately use" a single expression — which also reads better in a chain or an initializer.

---

## 29.9 Professional Insights

**Transfer elements between associative containers with `extract`/`insert(node)`/`merge`, never copy-erase-insert.** When you move a `map` or `set` entry from one container to another, the node-handle path relinks one node with zero allocations and never reconstructs the element — so even expensive-to-move or move-only mapped types transfer for the cost of a few pointer writes. The old "read the value, erase, insert elsewhere" pattern allocates a node, deallocates a node, and moves the value; the node handle does none of that. For bulk moves, `merge` is the one-call form.

**Use a node handle to re-key an element in place.** Changing a `map` key used to mean destroying and recreating the entry; `extract`, mutate `key()`, `insert(std::move(handle))` does it without touching the value or the heap. Reach for it whenever an element's key must change but its (possibly expensive) value should be preserved untouched.

**Default to `try_emplace` for "insert if absent" and `insert_or_assign` for "upsert."** `try_emplace` is the only insertion that *guarantees* it will not construct the value or move from your arguments when the key already exists — making it the correct, safe choice when those arguments are move-only types you still need on failure. `insert_or_assign` is the correct upsert: it avoids `operator[]`'s default-construction requirement and, unlike `operator[]`, tells you whether it inserted or overwrote. Choosing between them by intent eliminates a whole category of "my `unique_ptr` got emptied even though nothing was inserted" bugs.

**Capture the reference that `emplace_back` now returns.** Writing `auto& x = v.emplace_back(...)` and operating on `x` is clearer and cheaper than `emplace_back(...)` followed by `v.back()`, and it removes any chance of accidentally touching the wrong element. Make it the default spelling whenever you create an element and then immediately use it.
