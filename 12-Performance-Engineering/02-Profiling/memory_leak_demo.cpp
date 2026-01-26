/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Memory Leak Demo
 * Description: Intentionally leaks memory to demonstrate Profiling Tools.
 * 
 * INSTRUCTIONS:
 * Compile: g++ -g memory_leak_demo.cpp -o leak_app
 * Run with Valgrind: valgrind --leak-check=full ./leak_app
 */

#include <iostream>
#include <vector>

class LeakyObject {
    int* data;
public:
    LeakyObject(int size) {
        data = new int[size]; // Allocated on Heap
        std::cout << "Allocated " << size * sizeof(int) << " bytes.\n";
    }

    // MISSING DESTRUCTOR!
    // ~LeakyObject() { delete[] data; }
};

void create_leak() {
    LeakyObject* obj = new LeakyObject(100);
    // obj is never deleted, and inside obj, 'data' is never deleted.
    // 'obj' pointer is lost when function returns.
}

int main() {
    std::cout << "Starting Leak Demo...\n";
    
    for (int i = 0; i < 5; ++i) {
        create_leak();
    }

    std::cout << "Finished. Check Valgrind output.\n";
    return 0;
}

