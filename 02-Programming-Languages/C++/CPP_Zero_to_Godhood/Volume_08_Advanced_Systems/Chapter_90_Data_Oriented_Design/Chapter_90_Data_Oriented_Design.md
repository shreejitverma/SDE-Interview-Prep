# Chapter 90: Data-Oriented Design — AoS vs SoA and Hot/Cold Splitting

Object-oriented design organises code around *objects* that bundle data with behaviour; **data-oriented design (DOD)** organises it around the *transformations* the program actually performs on bulk data, laying memory out to match. The shift matters because, as Chapter 87 established, memory-bound performance is governed by what the cache fetches — and the natural OO layout (an array of fat objects) routinely drags unused bytes through cache, defeats prefetching, and blocks vectorisation. This chapter develops the two central DOD techniques, struct-of-arrays and hot/cold splitting, with the cost model that says exactly when each pays.

## Chapter Roadmap

- 90.1 The Data-Oriented Mindset
- 90.2 Array-of-Structs vs Struct-of-Arrays
- 90.3 The Cost Model: Why SoA Wins (and When It Doesn't)
- 90.4 Hot/Cold Splitting
- 90.5 SoA and Vectorisation
- 90.6 Practical Patterns and Hazards

---

## 90.1 The Data-Oriented Mindset

DOD starts from a different question than OOP. OOP asks "what *is* this entity and what can it do?" and produces a class bundling all of an entity's data behind methods. DOD asks "what *transformations* does this program run over many items, and how should the data be laid out so those run fast?" The unit of thought is the *batch operation over a collection*, not the individual object.

> **Why this matters.** The OO instinct — one class per entity, an array of those objects — optimises for *conceptual clarity* and for accessing *one whole object at a time*. But hot loops rarely touch one whole object; they touch *one or two fields across thousands of objects* (update every particle's position, sum every order's quantity). For that pattern the OO layout is pessimal: each object access pulls a full cache line of mostly-irrelevant fields. DOD aligns the memory layout with the *actual access pattern* of the hot loop. It is not anti-OOP everywhere — it is the recognition that the layout serving the cache is often not the layout serving the conceptual model, and on the hot path the cache wins.

---

## 90.2 Array-of-Structs vs Struct-of-Arrays

The same logical data has two physical layouts:

```cpp
// Min standard: C++11. Array-of-Structs (AoS) — the natural OO layout.
struct Particle { float x, y, z; float vx, vy, vz; float mass; };  // 28 bytes
std::vector<Particle> aos(N);
// Updating only positions touches x,y,z but loads vx,vy,vz,mass too (wasted bandwidth):
for (auto& p : aos) { p.x += p.vx; p.y += p.vy; p.z += p.vz; }

// Min standard: C++11. Struct-of-Arrays (SoA) — fields stored in parallel arrays.
struct Particles {
    std::vector<float> x, y, z, vx, vy, vz, mass;
};
Particles soa;   // each field is its own contiguous array
// Updating positions streams only x,y,z,vx,vy,vz — no mass, perfectly contiguous, vectorisable:
for (size_t i = 0; i < N; ++i) {
    soa.x[i] += soa.vx[i]; soa.y[i] += soa.vy[i]; soa.z[i] += soa.vz[i];
}
```
*Listing 90.1 — The same data as AoS and SoA. The hot loop's access pattern decides which is faster.*

**AoS** stores each object's fields together: `[x0 y0 z0 vx0 ... mass0][x1 y1 ...]`. **SoA** stores each field's values together: `[x0 x1 x2 ...][y0 y1 ...]...`.

> **Why this matters.** This is the foundational DOD decision and it is entirely about the access pattern. If the hot loop reads *most* fields of *one* object at a time (e.g. fully process one particle including collision, render, AI), AoS keeps that object's fields on one or two cache lines — AoS wins. If the hot loop reads *few* fields across *many* objects (the position update above), SoA streams exactly the needed fields contiguously while AoS wastes ~60% of every fetched line on `mass` and unread velocities — SoA wins, often 2–4×.

---

## 90.3 The Cost Model: Why SoA Wins (and When It Doesn't)

| Factor | AoS | SoA |
|---|---|---|
| Access one whole object | One/two lines — efficient | Scattered across all field arrays — many lines |
| Access one field over many objects | Wastes line on unread fields | Streams only that field — efficient |
| Prefetcher behaviour | One stream, but low useful density | One dense stream per field array |
| Vectorisation | Hard (fields interleaved/strided) | Easy (each field contiguous, unit stride) |
| Cache-line utilisation (hot loop) | Low when few fields used | High |
| Conceptual clarity / random whole-object access | Natural | Awkward; indices instead of objects |

> **Why this matters / cost model.** The decisive quantity is **cache-line utilisation**: of the 64 bytes a miss fetches, what fraction does the loop actually use? For the position update on AoS particles, only 24 of 28 object bytes are even relevant and the loop uses just 12 (x,y,z) plus reads 12 (vx,vy,vz) — but the line also drags `mass` and crosses object boundaries awkwardly. SoA pushes utilisation toward 100% for the fields in use. SoA's costs: accessing a *whole* object now touches seven different arrays (seven potential misses), random insertion/deletion must update every array, and the code is less readable (you manipulate parallel indices, not objects). The rule: **SoA for bulk field-wise transforms; AoS for whole-object, random-access work.** Many high-performance systems use *both* — AoS for the editor/logic view, SoA for the simulation hot loop.

---

## 90.4 Hot/Cold Splitting

Even within one object, fields differ in access frequency. **Hot/cold splitting** separates frequently-accessed ("hot") fields from rarely-accessed ("cold") ones so the hot fields pack densely into cache and the cold ones do not pollute it.

```cpp
// Min standard: C++11.
// BEFORE: cold debug/audit fields inflate the object, wasting cache on every hot scan.
struct OrderBad {
    uint64_t id; double price; uint32_t qty;        // HOT: touched every match
    char     trader_note[128];                      // COLD: touched only on audit
    std::string source_system;                      // COLD
};

// AFTER: hot fields dense; cold fields behind a pointer (one indirection, only when needed).
struct OrderCold { char trader_note[128]; std::string source_system; };
struct OrderGood {
    uint64_t id; double price; uint32_t qty;        // hot part fits in one cache line
    OrderCold* cold = nullptr;                       // cold data out-of-line
};
```
*Listing 90.2 — Hot/cold splitting moves rarely-used fields out of the dense hot array.*

> **Why this matters / cost model.** A hot scan over `OrderBad` fetches the 128-byte note and the string on every line even though the matching engine never reads them — the object spans multiple cache lines and the useful fields (id, price, qty) are diluted. Splitting the cold fields behind a pointer shrinks the hot object so several fit per cache line, multiplying the effective scan rate. The cost is one pointer indirection *when you actually need* the cold data (rare, by construction) and the management of the separate allocation. Hot/cold splitting is the per-field analogue of SoA and applies even when full SoA is impractical: identify the fields the hot loop touches, and evict the rest.

---

## 90.5 SoA and Vectorisation

SoA is the layout the auto-vectoriser (Chapter 92) wants. A SIMD instruction operates on N contiguous lanes; SoA's per-field contiguous arrays present exactly that, with unit stride and (with care) alignment. AoS forces the vectoriser to *gather* strided fields (`x` every 28 bytes), which is slow or impossible, so AoS loops often stay scalar.

> **Why this matters.** The SoA/vectorisation synergy compounds the win: SoA already improves cache utilisation, and on top of that it *enables* vectorisation that AoS blocks, so a SoA hot loop can be both more cache-efficient *and* process 4–16 lanes per instruction. This is why physics engines, particle systems, and numeric kernels overwhelmingly use SoA in their inner loops. Conversely, attempting to SIMD-optimise an AoS hot loop usually fails to vectorise and the engineer wrongly concludes "SIMD doesn't help here" — when the real problem is the layout.

---

## 90.6 Practical Patterns and Hazards

- **Entity-Component-Systems (ECS):** the dominant DOD architecture in games — entities are IDs, components are stored in SoA arrays per type, and systems are batch transforms over those arrays. It is hot/cold splitting and SoA generalised to a whole engine.
- **Indices over pointers:** SoA naturally uses integer indices into parallel arrays rather than object pointers; indices are smaller (often 32-bit), survive array reallocation (Chapter 83's `vector` growth invalidates pointers but not indices), and are cache-friendly.
- **`std::experimental::simd` / explicit SoA types:** libraries and the C++26 `std::simd` work naturally over SoA.

**Hazards:**

- **Premature DOD.** Restructuring cold code to SoA buys nothing and costs readability. Apply DOD only to *measured* hot, memory-bound loops.
- **Keeping parallel arrays consistent.** SoA insert/erase must touch every array; a bug that updates one array but not another corrupts the logical record. Encapsulate the arrays behind a class that maintains the invariant.
- **Whole-object access regression.** If a code path later needs whole objects frequently, SoA's scattered access may regress it; profile both paths.

> **The discipline.** Data-oriented design is the bridge between the cache model (Chapter 87) and real code: it asks you to design the *memory layout* around the hot loop's *access pattern* rather than around the conceptual object model. Choose AoS for whole-object random access, SoA for field-wise bulk transforms, and hot/cold splitting whenever a few fields dominate access — always driven by a profiler showing the loop is memory-bound. With the data laid out right, the next two chapters make the *code* over that data fast: removing unpredictable branches, then vectorising.
