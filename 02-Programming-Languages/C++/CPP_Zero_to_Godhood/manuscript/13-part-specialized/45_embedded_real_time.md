# Chapter 45: Embedded and Real-Time Systems

> *Where every microsecond counts, and a crash can cost millions.*

Writing C++ for a web backend is forgiving. If a function takes 100 milliseconds instead of 50, the user barely notices. If memory leaks slightly, the server restarts once a month.

Writing C++ for an airbag deployment system, a pacemaker, or a High-Frequency Trading (HFT) algorithm is entirely different. In these domains, we enter the realm of **Real-Time Programming**.

---

## 45.1 What is "Real-Time"?

"Real-Time" does not necessarily mean "Fast". It means **Deterministic**. A system is real-time if its correctness depends not only on the logical result, but on the *time* the result is delivered.

*   **Soft Real-Time:** Missing a deadline degrades the system, but is not fatal. (e.g., A video game dropping from 60 FPS to 50 FPS).
*   **Hard Real-Time:** Missing a deadline is a catastrophic failure. (e.g., A car's anti-lock braking system calculating the brake pressure 1 millisecond too late).

To achieve Hard Real-Time in C++, you must eliminate all sources of non-determinism.
1.  **No Dynamic Memory Allocation:** You cannot call `new` or `malloc`. They invoke the OS kernel, which takes an unpredictable amount of time to find free memory. Everything must be pre-allocated on the stack or in global memory pools.
2.  **No Exceptions:** Throwing an exception (`throw std::runtime_error`) unwinds the stack, which is highly unpredictable. Use `std::expected` or error codes instead.
3.  **No Blocking I/O:** You cannot wait for a hard drive or a network socket.

## 45.2 Bare-Metal vs RTOS

In embedded systems, you have two choices for your environment:

1.  **Bare-Metal:** There is no operating system. Your `main()` function is the only thing running on the microcontroller. You control every single cycle of the CPU.
2.  **RTOS (Real-Time Operating System):** Systems like FreeRTOS or VxWorks. Unlike Linux or Windows, an RTOS guarantees that a high-priority thread will interrupt a low-priority thread within a mathematically guaranteed number of microseconds.

## 45.3 Hardware Control: `volatile` and Memory-Mapped I/O

On an Arduino or a custom PCB, how do you turn on an LED? There is no `turn_on_led()` API provided by an OS.

You must talk directly to the hardware using **Memory-Mapped I/O**. The hardware engineers wire a specific memory address (e.g., `0x40020000`) directly to a physical pin on the chip. Writing a `1` to that memory address sends 5 Volts down the physical wire.

```cpp
// Define the memory address specified in the hardware datasheet
#define GPIO_PIN_0 (*(volatile uint32_t*)0x40020000)

void turn_on_led() {
    GPIO_PIN_0 = 1; // Actually changes physical voltage!
}
```

Notice the **`volatile`** keyword. This is critical.
Normally, if you write:
```cpp
int x = 0;
while (x == 0) { /* wait */ }
```
The C++ optimizer will say: *"x is 0, and nothing in this loop changes x. I will optimize this into an infinite loop."*

But if `x` is mapped to a physical button, the user might press the button and change the memory in hardware! 
Adding `volatile` tells the compiler: *"Do not optimize this. The value might change magically due to outside hardware forces. Read from RAM every single time."*

## 45.4 High-Frequency Trading (HFT)

HFT systems are the extreme edge of C++ performance. Firms spend millions of dollars to execute stock trades in under 500 **nanoseconds**. 

In HFT, traditional C++ optimizations are not enough. 

### Kernel Bypass
Normally, when a packet arrives on the Network Interface Card (NIC), it fires a hardware interrupt, the Linux kernel pauses your program, copies the packet from the NIC to kernel space, copies it from kernel space to user space, and wakes your program up. This takes roughly 5-10 microseconds.

In HFT, this is too slow. Engineers use **Kernel Bypass** (e.g., Solarflare OpenOnload). The C++ program maps its memory directly to the NIC's ring buffer. When a packet arrives, it appears instantly in the C++ array, completely bypassing the Linux Kernel.

### Cache Warm-up
If a CPU sits idle, it powers down slightly (C-states) to save energy. When a trade packet finally arrives, it takes microseconds for the CPU to wake back up to maximum frequency.
HFT programs constantly run "dummy" calculations in infinite `while` loops while waiting, just to keep the CPU physically hot and the L1 cache filled with the trading algorithm.

## 45.5 Safety Standards: MISRA C++ and AUTOSAR

When writing C++ for cars or airplanes, a single Undefined Behavior bug can cause loss of life.

To prevent this, the automotive and aerospace industries use strict linting standards like **MISRA C++** or **AUTOSAR**. These are massive rulebooks that ban dangerous C++ features.

Examples of MISRA Rules:
*   **Rule 5-0-15:** Array indexing shall be the only acceptable form of pointer arithmetic. (No `ptr++`).
*   **Rule 18-4-1:** Dynamic heap memory allocation shall not be used.
*   **Rule 6-5-6:** A loop control variable shall only be modified in the iteration expression.

```cpp
// MISRA-Compliant Array 
// (No dynamic memory, explicit bounds checking)
template <typename T, size_t N>
class SafeVector {
    T data[N];
    size_t count = 0;
public:
    bool push_back(const T& val) noexcept {
        if (count >= N) return false;
        data[count++] = val;
        return true;
    }
};
```

---

We have mastered how C++ runs on servers and how it runs on bare metal. But how do we write C++ that runs on Windows, macOS, Linux, and iOS all at the same time? We move to **Chapter 46: Cross-Platform Development and Cloud**.
