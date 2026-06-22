# Chapter 118: Real-Time Audio and Signal Processing

Real-time audio is hard real-time with an unforgiving deadline: the audio hardware demands a buffer of samples every few milliseconds, and if your callback is even *once* late — by a single missed deadline — the user hears a click, pop, or dropout. There is no retry. This makes the audio callback the strictest application of the determinism discipline in this entire book: it codifies, as inviolable rules, exactly what Volume 8 argued for the hot path. This chapter covers those rules, the lock-free communication that enforces them, and the SIMD and buffering techniques that make DSP fast.

## Chapter Roadmap

- 118.1 The Audio Callback and Its Deadline
- 118.2 The Golden Rules
- 118.3 Lock-Free UI↔Audio Communication
- 118.4 SIMD for DSP
- 118.5 Double Buffering for Block Processing
- 118.6 The Real-Time Audio Discipline

---

## 118.1 The Audio Callback and Its Deadline

Audio hardware processes a continuous stream of samples (e.g. 48,000 per second). It requests them in small **buffers** (often 64–512 samples) through a **callback** your code provides; the callback runs on a high-priority real-time thread, must fill the buffer, and must *return before the hardware needs the next one* — a deadline of perhaps 1–10 ms. Miss it, and the hardware plays whatever is in the (unfilled) buffer: silence or stale data, heard as a glitch.

> **Why this matters.** The audio callback is a **hard real-time** deadline with a brutal failure mode — unlike a slow web request (annoying but recoverable), a late audio callback produces an *audible, unrecoverable* artifact. This makes audio the purest test of the determinism discipline (Chapter 106): the callback's *worst-case* execution time, not its average, is all that matters, because a single overrun is a failure. Every technique in this book that *reduces variance* — no allocation, no locks, no syscalls, no page faults, cache-warm data — is, in audio, not an optimization but a *correctness requirement*. The audio thread is the hot path with the rules made absolute.

---

## 118.2 The Golden Rules

In the audio callback, the rules are inviolable — **thou shalt not**:

1. **Block** — no mutexes, no condition variables, no `std::lock`. A mutex held by another (lower-priority, possibly preempted) thread can stall the audio thread past its deadline — the priority-inversion convoy of Chapter 95.
2. **Allocate** — no `malloc`/`new`/`delete`, no growing containers. Allocation can hit the slow path (a lock, an `mmap`, a page fault — Chapters 79, 88) for an unbounded time.
3. **Perform I/O or syscalls** — no file reads, no `printf`, no logging. A syscall can block and costs an unpredictable amount (Chapter 98).

```cpp
// Min standard: C++11. The audio callback: pre-allocated, lock-free, no I/O, bounded work.
void audio_callback(float* out, int num_frames) {
    // ALLOWED: arithmetic, reading pre-allocated buffers, lock-free queue pop, SIMD.
    for (int i = 0; i < num_frames; ++i)
        out[i] = process_sample(input_[i]);     // bounded, allocation-free, no locks
    // FORBIDDEN here: new/malloc, std::mutex, file/printf, anything unbounded.
}
```
*Listing 118.1 — The audio callback obeys the golden rules: bounded, lock-free, allocation-free, no I/O.*

> **Why this matters.** These three rules are precisely the hot-path checklist of Chapter 106, elevated from "good practice" to "law," because audio's deadline is hard. Each forbidden operation is a *source of unbounded latency*: a mutex can block indefinitely (priority inversion), an allocation can fault, a syscall can sleep — and any one blowing the millisecond deadline is an audible failure. The discipline is identical to the trading hot path (Chapter 106): everything the callback needs is *pre-allocated* before audio starts, communication with other threads is *lock-free*, and all heavy or variable work (loading a sample from disk, logging, UI updates) happens on *other* threads. Audio engineers internalise these rules as reflexively as the rules of arithmetic.

---

## 118.3 Lock-Free UI↔Audio Communication

The audio thread cannot lock, but it must still communicate with the rest of the application — the UI thread changes parameters (volume, filter cutoff), and the audio thread reports levels back. This crossing *must* be lock-free, and the canonical tool is the **single-producer single-consumer (SPSC) ring buffer** (Chapter 77).

```cpp
// Min standard: C++11. A wait-free SPSC queue: UI thread writes params, audio thread reads them.
// (See Chapter 77 for the full implementation.)
//   UI thread (producer):    params_queue.push(new_cutoff);     // release store on tail index
//   Audio thread (consumer): if (params_queue.pop(cutoff)) ...  // acquire load on tail index
// Atomic head/tail indices, pre-allocated buffer, NO locks -> wait-free for both threads.
```
*Listing 118.2 — UI↔audio parameter passing via a wait-free SPSC ring (Chapter 77). No locks cross the boundary.*

> **Why this matters / cost model.** The SPSC ring is exactly right here because it is *wait-free for both parties* (Chapter 77): the audio thread's `pop` is a bounded, lock-free, allocation-free operation — an acquire load and a few index updates — so it can never block the callback. Parameter changes from the UI are *published* via the ring's release/acquire ordering (Chapter 76), so the audio thread sees fully-formed updates without a lock. For the reverse direction (audio → UI, e.g. level meters), another SPSC ring or a `std::atomic<float>` written by the audio thread and read by the UI suffices. The cardinal sin is a shared parameter behind a mutex — it works 99.99% of the time and then the UI thread holds the lock during a context switch, the audio thread blocks, and the user hears a pop. Lock-free communication is not an optimization here; it is the only correct mechanism.

---

## 118.4 SIMD for DSP

Digital signal processing — filters, FFTs, gain, mixing — is pure, regular arithmetic over sample arrays, which makes it ideal for **SIMD** (Chapter 92): process 4, 8, or 16 samples per instruction.

```cpp
// Min standard: C++11 + AVX (non-portable: x86). Process 8 samples at once.
#include <immintrin.h>
// Apply gain to 8 samples in one instruction (the inner loop of a mixer/EQ):
// __m256 samples = _mm256_loadu_ps(input + i);
// __m256 gained  = _mm256_mul_ps(samples, _mm256_set1_ps(gain));
// _mm256_storeu_ps(output + i, gained);
// A biquad filter (the EQ workhorse) similarly processes multiple channels in parallel lanes.
```
*Listing 118.3 — SIMD DSP: 8 samples per instruction. AVX intrinsics are x86-specific (Chapter 92).*

> **Why this matters / cost model.** DSP is the compute-bound, data-parallel workload SIMD was made for (Chapter 92): a gain stage, a mixer, or an FIR filter applies the same arithmetic to every sample with no data-dependent branches and unit stride — so it auto-vectorises well and benefits hugely from explicit SIMD when the compiler needs help (`__restrict` pointers, fixed loop bounds). The **biquad filter** — the building block of equalizers — and the FFT are the canonical kernels, and processing multiple audio channels in parallel SIMD lanes is a natural fit (Chapter 90's SoA layout: store all channels' sample-*i* together). SIMD lets the bounded callback do *more* DSP within its deadline, which is the whole game — a richer effects chain that still fits in the millisecond budget. This is the compute cost model (Chapter 85) serving the real-time deadline.

---

## 118.5 Double Buffering for Block Processing

Some DSP needs *blocks* of samples — an FFT for spectral analysis wants, say, 1024 samples at once — but audio arrives in small callback-sized chunks (64 samples). **Double buffering** (or a ring) bridges the mismatch: accumulate incoming chunks into buffer A; when it is full, hand A to a worker thread for the heavy block processing and switch to filling buffer B.

> **Why this matters.** Double buffering decouples the *real-time* constraint (the callback must return fast, every few ms) from the *block* constraint (the FFT needs a full window and takes longer than one callback). The callback does only the cheap, bounded work of copying its small chunk into the current buffer and, when a buffer fills, *signalling* a worker thread (lock-free, via a ring) to process the completed block — it never does the FFT itself, which could overrun the deadline. The worker thread, off the real-time path, can take its time. This is the same producer/consumer decoupling as the Disruptor (Chapter 78) and the hot/cold split (Chapter 106): keep the deadline-bound thread doing only bounded work, and push heavy or variable-latency work to a thread without a hard deadline.

---

## 118.6 The Real-Time Audio Discipline

| Constraint | Rule | Mechanism |
|---|---|---|
| Hard deadline | Bounded worst-case callback time | Pre-allocate everything |
| No blocking | No mutexes on the audio thread | SPSC rings for communication (Ch 77) |
| No allocation | No `new`/`malloc` in the callback | Object pools, fixed buffers (Ch 97) |
| No I/O | No file/log/syscall in the callback | Defer to worker threads (Ch 106) |
| Fit more DSP in budget | Maximise work per cycle | SIMD (Ch 92) |
| Block processing | Decouple block from callback size | Double buffering / worker thread |

> **The discipline.** Real-time audio is the determinism discipline of Volume 8 made *mandatory*: the audio callback is a hot path whose worst-case latency is a hard correctness requirement, because a single missed deadline is audible and unrecoverable. The golden rules — no blocking, no allocation, no I/O on the audio thread — are the hot-path checklist (Chapter 106) elevated to law; the SPSC ring (Chapter 77) is the only correct cross-thread mechanism; SIMD (Chapter 92) packs more DSP into the budget; and double buffering pushes block work off the deadline. Audio is where an engineer *cannot* be sloppy about jitter, which makes it the best training ground for every low-latency discipline in this book. The next chapter applies the same hard-real-time mindset to a domain where the deadline guards not a speaker but a motor: robotics.
