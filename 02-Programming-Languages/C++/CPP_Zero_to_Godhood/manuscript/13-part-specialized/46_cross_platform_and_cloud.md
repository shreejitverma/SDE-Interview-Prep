# Chapter 46: Cross-Platform and Cloud

> *Write once, compile everywhere.*

One of the great myths of C++ is that it is not portable. Java famously marketed itself as "Write Once, Run Everywhere," claiming that C++ was tied to specific hardware. 

This is false. Standard C++ is the most portable language on Earth. If you write purely Standard C++ (no POSIX headers, no Windows APIs), your code will compile on a Windows PC, a Linux server, an iPhone, an Android tablet, a Tesla dashboard, and inside a web browser.

The challenge is not the language; the challenge is the *toolchain*.

---

## 46.1 The Cross-Compilation Model

If you are on an Intel Mac and you want to compile a C++ app for an ARM Android phone, you cannot use your standard `g++`. You must use a **Cross-Compiler**—a compiler that runs on Architecture A but produces machine code for Architecture B.

Managing cross-compilers manually is excruciating. This is why CMake (Chapter 39) is mandatory. You provide CMake with a **Toolchain File**, which tells it exactly where the Android compiler, linker, and sysroot (system headers) are located.

## 46.2 C++ on Mobile (iOS and Android)

Why write mobile apps in C++ instead of Swift or Kotlin? 
If you are building a game (Unreal Engine), a physics simulation, or a complex audio processing app, writing the core logic in C++ allows you to share 90% of your codebase between iOS and Android.

### iOS (Objective-C++)
Apple makes this incredibly easy. Objective-C and C++ can be mixed in the same file (an `.mm` file). You can instantiate a C++ `std::vector` right next to an iOS `UIView`.

### Android (The NDK and JNI)
Android is heavily reliant on the Java Virtual Machine. To run C++ on Android, you use the **Android NDK (Native Development Kit)**.
To bridge the gap between Java and C++, you use the **Java Native Interface (JNI)**.

Crossing the JNI boundary is expensive, so you want to keep as much logic in C++ as possible, only crossing back to Java to update the UI.

```cpp
#include <jni.h>
#include <string>

// A JNI bridge function. Note the strict naming convention.
extern "C" JNIEXPORT jstring JNICALL
Java_com_godhood_app_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    std::string cpp_string = "Calculated in C++!";
    
    // Convert C++ string to Java String
    return env->NewStringUTF(cpp_string.c_str());
}
```

## 46.3 WebAssembly (C++ in the Browser)

For decades, JavaScript was the only language that could run inside a web browser. If you had a massive C++ video editing library, you had to rewrite it in JavaScript.

**WebAssembly (Wasm)** changed everything. It is a binary instruction format for a stack-based virtual machine, supported by all major browsers. It runs at near-native speed.

Using a tool called **Emscripten**, you can compile your C++ code directly into a `.wasm` file.

```cpp
// main.cpp
#include <emscripten/emscripten.h>

extern "C" {
    // EMSCRIPTEN_KEEPALIVE tells the compiler not to strip this function
    // out during optimization, making it visible to JavaScript.
    EMSCRIPTEN_KEEPALIVE
    int add(int a, int b) {
        return a + b;
    }
}
```

To compile:
```bash
emcc main.cpp -o index.html -s WASM=1
```
This generates an HTML file, a JavaScript glue file, and the binary `.wasm` file. Your C++ code is now running inside Google Chrome.

## 46.4 C++ in the Cloud

C++ is rarely used for standard CRUD (Create, Read, Update, Delete) web APIs. Languages like Go, Node.js, and Python are better suited for that due to their massive web ecosystems.

However, when you need ultra-high-throughput microservices or highly optimized Serverless functions, C++ shines.

### High-Performance Microservices
Frameworks like **Drogon** (consistently ranked as one of the fastest web frameworks in the world on TechEmpower benchmarks) or **userver** allow you to build asynchronous, non-blocking HTTP servers in C++.

### Serverless (AWS Lambda)
Cloud providers charge you based on memory usage and execution time. 
If a Java Lambda function suffers a 200ms "Cold Start" (the time it takes to boot the JVM), and a Python function takes 50ms to process a request, you pay for that time.

A C++ Lambda function has virtually zero cold start time and executes in single-digit milliseconds. At massive scale, rewriting an AWS Lambda function in C++ can save a company millions of dollars in AWS bills.

```cpp
#include <aws/lambda-runtime/runtime.h>

// The entry point for the AWS Lambda
aws::lambda_runtime::invocation_response my_handler(aws::lambda_runtime::invocation_request const& req) {
    // Process JSON payload...
    return aws::lambda_runtime::invocation_response::success("Processed rapidly!", "application/json");
}

int main() {
    // Starts the event loop
    aws::lambda_runtime::run_handler(my_handler);
    return 0;
}
```

---

We have covered Networking, Embedded Systems, Mobile, and the Cloud. But we have avoided the most visual aspect of programming: drawing pixels on a screen. We move to **Chapter 47: GUI and Graphics Programming**.
