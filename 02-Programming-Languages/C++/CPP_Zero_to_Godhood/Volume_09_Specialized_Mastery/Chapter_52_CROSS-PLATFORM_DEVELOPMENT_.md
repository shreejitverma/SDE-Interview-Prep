# CROSS-PLATFORM DEVELOPMENT


# CROSS-PLATFORM DEVELOPMENT


Write once, run everywhere (Desktop, Web, Mobile).

### 21.1 WebAssembly (Wasm) with Emscripten
Compiling C++ to run in the browser.

```bash
emcc main.cpp -o index.html -s WASM=1
```

```cpp
#include <emscripten/emscripten.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int add(int a, int b) {
        return a + b; // callable from JavaScript
    }
}
```

### 21.2 Mobile C++ (Android NDK & JNI)
Integrating C++ with Java/Kotlin.

```cpp
#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapp_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    return env->NewStringUTF("Hello from C++");
}
```

---


---
