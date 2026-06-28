# Python 3.5: Native Async/Await Coroutines and Matrix Operations


### 11.1 PEP 492: Native Coroutines
Introduced in Python 3.5, **PEP 492** separated coroutines from generators by introducing the `async def` and `await` syntax.
*   **Native Coroutines (`PyCoroObject`)**: Functions declared with `async def` return a native coroutine object instead of a generator. Coroutines do not support standard iterator methods (`__next__`); instead, they implement the `__await__` method, which returns an iterator used by the event loop to drive execution.

### 11.2 The Event Loop and OS Selectors
The event loop is a single-threaded runtime manager that schedules and executes concurrent operations.
*   **I/O Multiplexing**: The loop utilizes the standard library `selectors` module to monitor I/O sockets. This wraps high-performance OS-specific polling mechanisms:
    - `epoll` (Linux)
    - `kqueue` (macOS / BSD)
    - `select` (Fallback platform wrapper)
*   **Non-Blocking Scheduling**: When a task awaits an I/O operation (like reading from a socket), the event loop registers the socket's file descriptor with the OS selector and suspends the task. While the socket is waiting, the loop executes other tasks. Once the selector signals that the socket is ready, the event loop resumes the suspended task.

### 11.3 Task and Future State Machines
*   **Future**: Represents the eventual result of an asynchronous operation. It tracks state: `PENDING`, `CANCELLED`, or `FINISHED`.
*   **Task**: A subclass of `Future` that wraps a coroutine object. The event loop drives the task by calling its `step()` method:
    1.  `step()` invokes the coroutine's `send(None)` to run it.
    2.  The coroutine executes until it awaits another future, which raises a `YieldPoint` and returns control to the loop.
    3.  Once the awaited future completes, the loop calls `step()` again, passing the result back into the coroutine via `send(result)`.

```
Task Lifecycle State Machine:
  [ PENDING ] ---> ( Scheduled on Loop ) ---> [ RUNNING (step called) ]
       |                                             |
       | <----( Yields on await / Pending Future )---+
       |
       +-------> ( Finished / Exception ) ------> [ FINISHED ]
```

---

