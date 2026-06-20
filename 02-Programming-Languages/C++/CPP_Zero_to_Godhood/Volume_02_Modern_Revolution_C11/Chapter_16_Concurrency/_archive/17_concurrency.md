# Chapter 17: Concurrency & Multithreading

# CONCURRENCY & MULTITHREADING

C++11 brought a standard threading model.

## 1. Threads (`std::thread`)

```cpp
#include <thread>

void task() { /*...*/ }

int main() {
    std::thread t(task);
    t.join(); // Wait for finish
    // t.detach(); // Or let it run freely
}
```

## 2. Mutexes & Locking

Avoid data races.

*   `std::mutex`: Basic lock.
*   `std::lock_guard`: RAII wrapper (locks on construction, unlocks on destruction).
*   `std::unique_lock`: Flexible RAII wrapper (can unlock manually).

```cpp
std::mutex mtx;
void safe() {
    std::lock_guard<std::mutex> lock(mtx);
    // critical section
}
```

## 3. Condition Variables

Wait for a condition to be true.

```cpp
std::condition_variable cv;
std::mutex mtx;
bool ready = false;

// Waiter
std::unique_lock<std::mutex> lk(mtx);
cv.wait(lk, []{ return ready; });

// Notifier
{
    std::lock_guard<std::mutex> lk(mtx);
    ready = true;
}
cv.notify_one();
```

## 4. Futures & Promises

Asynchronous result retrieval.

*   `std::async`: Runs a function asynchronously.
*   `std::future`: Holds the result.

```cpp
auto f = std::async(std::launch::async, []{ return 42; });
int result = f.get(); // Blocks until ready
```

## 5. Atomics (`std::atomic`)

Lock-free operations for basic types.

```cpp
std::atomic<int> counter(0);
counter++; // Thread-safe increment
```

***
### Professional Insights: Concurrency Depth

#### 1. Thread Lifecycle and Exceptions

If a `std::thread` object is destroyed while it is still "joinable" (not joined or detached), `std::terminate()` is called.
*   **Safety**: Always use a wrapper or ensure `join()`/`detach()` is called in all exit paths, including exception handlers.
*   **`std::jthread` (C++20)**: Automatically joins on destruction, solving this problem.

#### 2. Advanced Locking Strategies

*   **`std::scoped_lock` (C++17)**: Locks multiple mutexes simultaneously using a deadlock-avoidance algorithm (replaces `std::lock`).
*   **`std::shared_mutex` (C++17)**: Allows multiple readers or one writer (Reader-Writer Lock).
*   **Lock Strategies**:
    *   `std::adopt_lock`: Assume the calling thread already owns the mutex.
    *   `std::defer_lock`: Do not lock the mutex on construction.
    *   `std::try_to_lock`: Attempt to lock without blocking.

#### 3. Semaphores (C++20)

A semaphore is a synchronization primitive that maintains a counter.
*   **`std::counting_semaphore<N>`**: Allows up to $N$ concurrent accesses.
*   **`std::binary_semaphore`**: Alias for `counting_semaphore<1>`.
*   **Usage**: Useful for limiting access to a pool of resources (e.g., database connections).

#### 4. Thread Local Storage (TLS)

The `thread_local` keyword ensures that each thread has its own unique instance of a variable.
```cpp
thread_local int thread_id = 0; // Each thread gets its own copy
```

***

## Professional Insights: std::atomics

Section 55.1: atomic types
Each instantiation and full specialization of the std::atomic template deﬁnes an atomic type. If one thread writes
to an atomic object while another thread reads from it, the behavior is well-deﬁned (see memory model for details
on data races)
In addition, accesses to atomic objects may establish inter-thread synchronization and order non-atomic memory
accesses as speciﬁed by std::memory_order.
std::atomic may be instantiated with any TriviallyCopyable type T. std::atomic is neither copyable nor
movable.
The standard library provides specializations of the std::atomic template for the following types:
1.
One full specialization for the type bool and its typedef name is deﬁned that is treated as a non-specialized
std::atomic<T> except that it has standard layout, trivial default constructor, trivial destructors, and
supports aggregate initialization syntax:
Typedef name
Full specialization
std::atomic_bool std::atomic<bool>
2)Full specializations and typedefs for integral types, as follows:
Typedef name
Full specialization
std::atomic_char
std::atomic<char>
std::atomic_char
std::atomic<char>
std::atomic_schar
std::atomic<signed char>
std::atomic_uchar
std::atomic<unsigned char>
std::atomic_short
std::atomic<short>
std::atomic_ushort
std::atomic<unsigned short>
std::atomic_int
std::atomic<int>
std::atomic_uint
std::atomic<unsigned int>
std::atomic_long
std::atomic<long>
std::atomic_ulong
std::atomic<unsigned long>
std::atomic_llong
std::atomic<long long>
std::atomic_ullong
std::atomic<unsigned long long>
std::atomic_char16_t
std::atomic<char16_t>
std::atomic_char32_t
std::atomic<char32_t>
std::atomic_wchar_t
std::atomic<wchar_t>
std::atomic_int8_t
std::atomic<std::int8_t>
std::atomic_uint8_t
std::atomic<std::uint8_t>
std::atomic_int16_t
std::atomic<std::int16_t>
std::atomic_uint16_t
std::atomic<std::uint16_t>
std::atomic_int32_t
std::atomic<std::int32_t>
std::atomic_uint32_t
std::atomic<std::uint32_t>
std::atomic_int64_t
std::atomic<std::int64_t>
std::atomic_uint64_t
std::atomic<std::uint64_t>
std::atomic_int_least8_t
std::atomic<std::int_least8_t>
std::atomic_uint_least8_t std::atomic<std::uint_least8_t>

std::atomic_int_least16_t std::atomic<std::int_least16_t>
std::atomic_uint_least16_t std::atomic<std::uint_least16_t>
std::atomic_int_least32_t std::atomic<std::int_least32_t>
std::atomic_uint_least32_t std::atomic<std::uint_least32_t>
std::atomic_int_least64_t std::atomic<std::int_least64_t>
std::atomic_uint_least64_t std::atomic<std::uint_least64_t>
std::atomic_int_fast8_t
std::atomic<std::int_fast8_t>
std::atomic_uint_fast8_t
std::atomic<std::uint_fast8_t>
std::atomic_int_fast16_t
std::atomic<std::int_fast16_t>
std::atomic_uint_fast16_t std::atomic<std::uint_fast16_t>
std::atomic_int_fast32_t
std::atomic<std::int_fast32_t>
std::atomic_uint_fast32_t std::atomic<std::uint_fast32_t>
std::atomic_int_fast64_t
std::atomic<std::int_fast64_t>
std::atomic_uint_fast64_t std::atomic<std::uint_fast64_t>
std::atomic_intptr_t
std::atomic<std::intptr_t>
std::atomic_uintptr_t
std::atomic<std::uintptr_t>
std::atomic_size_t
std::atomic<std::size_t>
std::atomic_ptrdiff_t
std::atomic<std::ptrdiff_t>
std::atomic_intmax_t
std::atomic<std::intmax_t>
std::atomic_uintmax_t
std::atomic<std::uintmax_t>
Simple example of using std::atomic_int
```cpp
#include <iostream>       // std::cout
#include <atomic>         // std::atomic, std::memory_order_relaxed
#include <thread>         // std::thread

std::atomic_int foo (0);
void set_foo(int x) {
  foo.store(x,std::memory_order_relaxed);     // set value atomically
}
void print_foo() {
  int x;
  do {
    x = foo.load(std::memory_order_relaxed);  // get value atomically
  } while (x==0);
  std::cout << "foo: " << x << '\\n';
}
int main ()
{
  std::thread first (print_foo);
  std::thread second (set_foo,10);
  first.join();
  //second.join();
  return 0;
}
```

//output: foo: 10

## Professional Insights: Threading

Parameter
other
Details
Takes ownership of other, other doesn't own the thread anymore
func
args
Function to call in a separate thread
Arguments for func
Section 80.1: Creating a std::thread
In C++, threads are created using the std::thread class. A thread is a separate ﬂow of execution; it is analogous to
having a helper perform one task while you simultaneously perform another. When all the code in the thread is
executed, it terminates. When creating a thread, you need to pass something to be executed on it. A few things that
you can pass to a thread:
Free functions
Member functions
Functor objects
Lambda expressions
Free function example - executes a function on a separate thread (Live Example):
```cpp
#include <iostream>
#include <thread>

void foo(int a)
{
    std::cout << a << '\\n';
}
int main()
{
    // Create and execute the thread
    std::thread thread(foo, 10); // foo is the function to execute, 10 is the
                                 // argument to pass to it
    // Keep going; the thread is executed separately
    // Wait for the thread to finish; we stay here until it is done
    thread.join();
    return 0;
}
```

Member function example - executes a member function on a separate thread (Live Example):
```cpp
#include <iostream>
#include <thread>

class Bar
{
public:
    void foo(int a)
    {
        std::cout << a << '\\n';
    }
};

int main()
{
    Bar bar;
    // Create and execute the thread
    std::thread thread(&Bar::foo, &bar, 10); // Pass 10 to member function
    // The member function will be executed in a separate thread
    // Wait for the thread to finish, this is a blocking operation
    thread.join();
    return 0;
}
```

Functor object example (Live Example):
```cpp
#include <iostream>
#include <thread>

class Bar
{
public:
    void operator()(int a)
    {
        std::cout << a << '\\n';
    }
};
int main()
{
    Bar bar;
    // Create and execute the thread
    std::thread thread(bar, 10); // Pass 10 to functor object
    // The functor object will be executed in a separate thread
    // Wait for the thread to finish, this is a blocking operation
    thread.join();
    return 0;
}
```

Lambda expression example (Live Example):
```cpp
#include <iostream>
#include <thread>

int main()
{
    auto lambda = [](int a) { std::cout << a << '\\n'; };
    // Create and execute the thread
    std::thread thread(lambda, 10); // Pass 10 to the lambda expression
    // The lambda expression will be executed in a separate thread
    // Wait for the thread to finish, this is a blocking operation
    thread.join();

    return 0;
}
```

Section 80.2: Passing a reference to a thread
You cannot pass a reference (or const reference) directly to a thread because std::thread will copy/move them.
Instead, use std::reference_wrapper:
```cpp
void foo(int& b)
{
    b = 10;
}
int a = 1;
std::thread thread{ foo, std::ref(a) }; //'a' is now really passed as reference
thread.join();
std::cout << a << '\\n'; //Outputs 10
void bar(const ComplexObject& co)
{
    co.doCalculations();
}
ComplexObject object;
std::thread thread{ bar, std::cref(object) }; //'object' is passed as const&
thread.join();
std::cout << object.getResult() << '\\n'; //Outputs the result
```

Section 80.3: Using std::async instead of std::thread
std::async is also able to make threads. Compared to std::thread it is considered less powerful but easier to use
when you just want to run a function asynchronously.
Asynchronously calling a function
```cpp
#include <future>
#include <iostream>

unsigned int square(unsigned int i){
    return i*i;
}
int main() {
    auto f = std::async(std::launch::async, square, 8);
    std::cout << "square currently running\\n"; //do something while square is running
    std::cout << "result is " << f.get() << '\\n'; //getting the result from square
}
```

Common Pitfalls
std::async returns a std::future that holds the return value that will be calculated by the function. When
that future gets destroyed it waits until the thread completes, making your code eﬀectively single threaded.
This is easily overlooked when you don't need the return value:

```cpp
std::async(std::launch::async, square, 5);
//thread already completed at this point, because the returning future got destroyed
```

std::async works without a launch policy, so std::async(square, 5); compiles. When you do that the
system gets to decide if it wants to create a thread or not. The idea was that the system chooses to make a
thread unless it is already running more threads than it can run eﬃciently. Unfortunately implementations
commonly just choose not to create a thread in that situation, ever, so you need to override that behavior
with std::launch::async which forces the system to create a thread.
Beware of race conditions.
More on async on Futures and Promises
Section 80.4: Basic Synchronization
Thread synchronization can be accomplished using mutexes, among other synchronization primitives. There are
several mutex types provided by the standard library, but the simplest is std::mutex. To lock a mutex, you
construct a lock on it. The simplest lock type is std::lock_guard:
```cpp
std::mutex m;
void worker() {
    std::lock_guard<std::mutex> guard(m); // Acquires a lock on the mutex
    // Synchronized code here
} // the mutex is automatically released when guard goes out of scope
```

With std::lock_guard the mutex is locked for the whole lifetime of the lock object. In cases where you need to
manually control the regions for locking, use std::unique_lock instead:
```cpp
std::mutex m;
void worker() {
    // by default, constructing a unique_lock from a mutex will lock the mutex
    // by passing the std::defer_lock as a second argument, we
    // can construct the guard in an unlocked state instead and
    // manually lock later.
    std::unique_lock<std::mutex> guard(m, std::defer_lock);
    // the mutex is not locked yet!
    guard.lock();
    // critical section
    guard.unlock();
    // mutex is again released
}
```

More Thread synchronization structures
Section 80.5: Create a simple thread pool
C++11 threading primitives are still relatively low level. They can be used to write a higher level construct, like a
thread pool:
Version ≥ C++14
```cpp
struct tasks {
  // the mutex, condition variable and deque form a single
  // thread-safe triggered queue of tasks:
  std::mutex m;
  std::condition_variable v;
  // note that a packaged_task<void> can store a packaged_task<R>:

  std::deque<std::packaged_task<void()>> work;
  // this holds futures representing the worker threads being done:
  std::vector<std::future<void>> finished;
  // queue( lambda ) will enqueue the lambda into the tasks for the threads
  // to use.  A future of the type the lambda returns is given to let you get
  // the result out.
  template<class F, class R=std::result_of_t<F&()>>
  std::future<R> queue(F&& f) {
    // wrap the function object into a packaged task, splitting
    // execution from the return value:
    std::packaged_task<R()> p(std::forward<F>(f));
    auto r=p.get_future(); // get the return value before we hand off the task
    {
      std::unique_lock<std::mutex> l(m);
      work.emplace_back(std::move(p)); // store the task<R()> as a task<void()>
    }
    v.notify_one(); // wake a thread to work on the task
    return r; // return the future result of the task
  }
  // start N threads in the thread pool.
  void start(std::size_t N=1){
    for (std::size_t i = 0; i < N; ++i)
    {
      // each thread is a std::async running this->thread_task():
      finished.push_back(
        std::async(
          std::launch::async,
          [this]{ thread_task(); }
        )
      );
    }
  }
  // abort() cancels all non-started tasks, and tells every working thread
  // stop running, and waits for them to finish up.
  void abort() {
    cancel_pending();
    finish();
  }
  // cancel_pending() merely cancels all non-started tasks:
  void cancel_pending() {
    std::unique_lock<std::mutex> l(m);
    work.clear();
  }
  // finish enques a "stop the thread" message for every thread, then waits for them:
  void finish() {
    {
      std::unique_lock<std::mutex> l(m);
      for(auto&&unused:finished){
        work.push_back({});
      }
    }
    v.notify_all();
    finished.clear();
  }
  ~tasks() {
    finish();
  }

private:
  // the work that a worker thread does:
  void thread_task() {
    while(true){
      // pop a task off the queue:
      std::packaged_task<void()> f;
      {
        // usual thread-safe queue code:
        std::unique_lock<std::mutex> l(m);
        if (work.empty()){
          v.wait(l,[&]{return !work.empty();});
        }
        f = std::move(work.front());
        work.pop_front();
      }
      // if the task is invalid, it means we are asked to abort:
      if (!f.valid()) return;
      // otherwise, run the task:
      f();
    }
  }
};
```

tasks.queue( []{ return "hello world"s; } ) returns a std::future<std::string>, which when the tasks
object gets around to running it is populated with hello world.
You create threads by running tasks.start(10) (which starts 10 threads).
The use of packaged_task<void()> is merely because there is no type-erased std::function equivalent that stores
move-only types. Writing a custom one of those would probably be faster than using packaged_task<void()>.
Live example.
Version = C++11
In C++11, replace result_of_t<blah> with typename result_of<blah>::type.
More on Mutexes.
Section 80.6: Ensuring a thread is always joined
When the destructor for std::thread is invoked, a call to either join() or detach() must have been made. If a
thread has not been joined or detached, then by default std::terminate will be called. Using RAII, this is generally
simple enough to accomplish:
```cpp
class thread_joiner
{
public:
    thread_joiner(std::thread t)
        : t_(std::move(t))
    { }
    ~thread_joiner()
    {
        if(t_.joinable()) {
            t_.join();
        }

    }
private:
    std::thread t_;
}
```

This is then used like so:
```cpp
 void perform_work()
 {
     // Perform some work
 }
 void t()
 {
     thread_joiner j{std::thread(perform_work)};
     // Do some other calculations while thread is running
 } // Thread is automatically joined here
```

This also provides exception safety; if we had created our thread normally and the work done in t() performing
other calculations had thrown an exception, join() would never have been called on our thread and our process
would have been terminated.
Section 80.7: Operations on the current thread
std::this_thread is a namespace which has functions to do interesting things on the current thread from function
it is called from.
Function
Description
get_id
Returns the id of the thread
sleep_for
Sleeps for a speciﬁed amount of time
sleep_until Sleeps until a speciﬁc time
yield
Reschedule running threads, giving other threads priority
Getting the current threads id using std::this_thread::get_id:
```cpp
void foo()
{
    //Print this threads id
    std::cout << std::this_thread::get_id() << '\\n';
}
std::thread thread{ foo };
thread.join(); //'threads' id has now been printed, should be something like 12556
foo(); //The id of the main thread is printed, should be something like 2420
```

Sleeping for 3 seconds using std::this_thread::sleep_for:
```cpp
void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

std::thread thread{ foo };
foo.join();
std::cout << "Waited for 3 seconds!\\n";
```

Sleeping until 3 hours in the future using std::this_thread::sleep_until:
```cpp
void foo()
{
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::hours(3));
}
std::thread thread{ foo };
thread.join();
std::cout << "We are now located 3 hours after the thread has been called\\n";
```

Letting other threads take priority using std::this_thread::yield:
```cpp
void foo(int a)
{
    for (int i = 0; i < al ++i)
        std::this_thread::yield(); //Now other threads take priority, because this thread
                                   //isn't doing anything important
    std::cout << "Hello World!\\n";
}
std::thread thread{ foo, 10 };
thread.join();
```

Section 80.8: Using Condition Variables
A condition variable is a primitive used in conjunction with a mutex to orchestrate communication between
threads. While it is neither the exclusive or most eﬃcient way to accomplish this, it can be among the simplest to
those familiar with the pattern.
One waits on a std::condition_variable with a std::unique_lock<std::mutex>. This allows the code to safely
examine shared state before deciding whether or not to proceed with acquisition.
Below is a producer-consumer sketch that uses std::thread, std::condition_variable, std::mutex, and a few
others to make things interesting.
```cpp
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <thread>

int main()
{
    std::condition_variable cond;
    std::mutex mtx;

    std::queue<int> intq;
    bool stopped = false;
    std::thread producer{[&]()
    {
        // Prepare a random number generator.
        // Our producer will simply push random numbers to intq.
        //
        std::default_random_engine gen{};
        std::uniform_int_distribution<int> dist{};
        std::size_t count = 4006;
        while(count--)
        {
            // Always lock before changing
            // state guarded by a mutex and
            // condition_variable (a.k.a. "condvar").
            std::lock_guard<std::mutex> L{mtx};
            // Push a random int into the queue
            intq.push(dist(gen));
            // Tell the consumer it has an int
            cond.notify_one();
        }
        // All done.
        // Acquire the lock, set the stopped flag,
        // then inform the consumer.
        std::lock_guard<std::mutex> L{mtx};
        std::cout << "Producer is done!" << std::endl;
        stopped = true;
        cond.notify_one();
    }};
    std::thread consumer{[&]()
    {
        do{
            std::unique_lock<std::mutex> L{mtx};
            cond.wait(L,[&]()
            {
                // Acquire the lock only if
                // we've stopped or the queue
                // isn't empty
                return stopped || ! intq.empty();
            });
            // We own the mutex here; pop the queue
            // until it empties out.
            while( ! intq.empty())
            {
                const auto val = intq.front();
                intq.pop();
                std::cout << "Consumer popped: " << val << std::endl;
            }
            if(stopped){
                // producer has signaled a stop

                std::cout << "Consumer is done!" << std::endl;
                break;
            }
        }while(true);
    }};
    consumer.join();
    producer.join();
    std::cout << "Example Completed!" << std::endl;
    return 0;
}
```

Section 80.9: Thread operations
When you start a thread, it will execute until it is ﬁnished.
Often, at some point, you need to (possibly - the thread may already be done) wait for the thread to ﬁnish, because
you want to use the result for example.
```cpp
int n;
std::thread thread{ calculateSomething, std::ref(n) };
//Doing some other stuff
//We need 'n' now!
//Wait for the thread to finish - if it is not already done
thread.join();
//Now 'n' has the result of the calculation done in the separate thread
std::cout << n << '\\n';
```

You can also detach the thread, letting it execute freely:
```cpp
std::thread thread{ doSomething };
//Detaching the thread, we don't need it anymore (for whatever reason)
thread.detach();
```

//The thread will terminate when it is done, or when the main thread returns
Section 80.10: Thread-local storage
Thread-local storage can be created using the thread_local keyword. A variable declared with the thread_local
speciﬁer is said to have thread storage duration.
Each thread in a program has its own copy of each thread-local variable.
A thread-local variable with function (local) scope will be initialized the ﬁrst time control passes through its
deﬁnition. Such a variable is implicitly static, unless declared extern.
A thread-local variable with namespace or class (non-local) scope will be initialized as part of thread startup.
Thread-local variables are destroyed upon thread termination.
A member of a class can only be thread-local if it is static. There will therefore be one copy of that variable
per thread, rather than one copy per (thread, instance) pair.

Example:
```cpp
void debug_counter() {
    thread_local int count = 0;
    Logger::log("This function has been called %d times by this thread", ++count);
}
```

Section 80.11: Reassigning thread objects
We can create empty thread objects and assign work to them later.
If we assign a thread object to another active, joinable thread, std::terminate will automatically be called before
the thread is replaced.
```cpp
#include <thread>

void foo()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
}
//create 100 thread objects that do nothing
std::thread executors[100];
// Some code
// I want to create some threads now
for (int i = 0;i < 100;i++)
{
    // If this object doesn't have a thread assigned
    if (!executors[i].joinable())
         executors[i] = std::thread(foo);
}
```


## Professional Insights: Mutexes

Section 85.1: Mutex Types
C++1x oﬀers a selection of mutex classes:
std::mutex - oﬀers simple locking functionality.
std::timed_mutex - oﬀers try_to_lock functionality
std::recursive_mutex - allows recursive locking by the same thread.
std::shared_mutex, std::shared_timed_mutex - oﬀers shared and unique lock functionality.
Section 85.2: std::lock
std::lock uses deadlock avoidance algorithms to lock one or more mutexes. If an exception is thrown during a call
to lock multiple objects, std::lock unlocks the successfully locked objects before re-throwing the exception.
```cpp
std::lock(_mutex1, _mutex2);
```

Section 85.3: std::unique_lock, std::shared_lock,
std::lock_guard
Used for the RAII style acquiring of try locks, timed try locks and recursive locks.
std::unique_lock allows for exclusive ownership of mutexes.
std::shared_lock allows for shared ownership of mutexes. Several threads can hold std::shared_locks on a
std::shared_mutex. Available from C++ 14.
std::lock_guard is a lightweight alternative to std::unique_lock and std::shared_lock.
```cpp
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <string>
#include <iostream>

class PhoneBook {
public:
    std::string getPhoneNo( const std::string & name )
    {
        std::shared_lock<std::shared_timed_mutex> l(_protect);
        auto it =  _phonebook.find( name );
        if ( it != _phonebook.end() )
            return (*it).second;
        return "";
    }
    void addPhoneNo ( const std::string & name, const std::string & phone )
    {
        std::unique_lock<std::shared_timed_mutex> l(_protect);
        _phonebook[name] = phone;
    }
    std::shared_timed_mutex _protect;
    std::unordered_map<std::string,std::string>  _phonebook;

};
```

Section 85.4: Strategies for lock classes: std::try_to_lock,
std::adopt_lock, std::defer_lock
When creating a std::unique_lock, there are three diﬀerent locking strategies to choose from: std::try_to_lock,
std::defer_lock and std::adopt_lock
1.
std::try_to_lock allows for trying a lock without blocking:
```cpp
{
    std::atomic_int temp {0};
    std::mutex _mutex;
    std::thread t( [&](){
        while( temp!= -1){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::unique_lock<std::mutex> lock( _mutex, std::try_to_lock);
            if(lock.owns_lock()){
                //do something
                temp=0;
            }
        }
    });
    while ( true )
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock( _mutex, std::try_to_lock);
        if(lock.owns_lock()){
            if (temp < INT_MAX){
                ++temp;
            }
            std::cout << temp << std::endl;
        }
    }
}
```

2.
std::defer_lock allows for creating a lock structure without acquiring the lock. When locking more than one
mutex, there is a window of opportunity for a deadlock if two function callers try to acquire the locks at the
same time:
```cpp
{
    std::unique_lock<std::mutex> lock1(_mutex1, std::defer_lock);
    std::unique_lock<std::mutex> lock2(_mutex2, std::defer_lock);
    lock1.lock()
    lock2.lock(); // deadlock here
    std::cout << "Locked! << std::endl;
    //...
}
```

With the following code, whatever happens in the function, the locks are acquired and released in appropriate
order:
```cpp
   {
       std::unique_lock<std::mutex> lock1(_mutex1, std::defer_lock);
       std::unique_lock<std::mutex> lock2(_mutex2, std::defer_lock);

       std::lock(lock1,lock2); // no deadlock possible
       std::cout << "Locked! << std::endl;
       //...
   }
```

3.
std::adopt_lock does not attempt to lock a second time if the calling thread currently owns the lock.
```cpp
{
    std::unique_lock<std::mutex> lock1(_mutex1, std::adopt_lock);
    std::unique_lock<std::mutex> lock2(_mutex2, std::adopt_lock);
    std::cout << "Locked! << std::endl;
    //...
}
```

Something to keep in mind is that std::adopt_lock is not a substitute for recursive mutex usage. When the lock goes
out of scope the mutex is released.
Section 85.5: std::mutex
std::mutex is a simple, non-recursive synchronization structure that is used to protect data which is accessed by
multiple threads.
```cpp
    std::atomic_int temp{0};
    std::mutex _mutex;
    std::thread t( [&](){
                      while( temp!= -1){
                          std::this_thread::sleep_for(std::chrono::seconds(5));
                          std::unique_lock<std::mutex> lock( _mutex);
                              temp=0;
                      }
                  });
    while ( true )
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::unique_lock<std::mutex> lock( _mutex, std::try_to_lock);
        if ( temp < INT_MAX )
            temp++;
        cout << temp << endl;
    }
```

Section 85.6: std::scoped_lock (C++ 17)
std::scoped_lock provides RAII style semantics for owning one more mutexes, combined with the lock avoidance
algorithms used by std::lock. When std::scoped_lock is destroyed, mutexes are released in the reverse order
from which they where acquired.
```cpp
{
    std::scoped_lock lock{_mutex1,_mutex2};
    //do something
}
```


***



#### 10.6.1 Active Object Pattern

Decouples method execution from invocation. The object owns a thread and a message queue.

```cpp
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ActiveObject {
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    bool done = false;

public:
    ActiveObject() {
        worker = std::thread([this] { run(); });
    }

    ~ActiveObject() {
        { std::lock_guard lock(mtx); done = true; }
        cv.notify_one();
        worker.join();
    }

    void invoke(std::function<void()> task) {
        std::lock_guard lock(mtx);
        tasks.push(std::move(task));
        cv.notify_one();
    }

private:
    void run() {
        while (true) {
            std::unique_lock lock(mtx);
            cv.wait(lock, [this] { return !tasks.empty() || done; });
            
            if (done && tasks.empty()) return;
            
            auto task = std::move(tasks.front());
            tasks.pop();
            lock.unlock();
            
            task(); // Execute
        }
    }
};
```



#### 10.6.2 Monitor Object (Thread-Safe Interface)

Ensure thread safety by locking in public methods and calling private implementation methods.

```cpp
class Monitor {
    mutable std::mutex mtx;
    int state = 0;

public:
    void update(int val) {
        std::lock_guard lock(mtx); // Lock here
        update_impl(val);
    }

private:
    // Expects lock to be held
    void update_impl(int val) {
        state = val;
    }
};
```

***

