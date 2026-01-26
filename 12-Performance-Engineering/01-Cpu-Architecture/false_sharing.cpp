/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: False Sharing (Performance Anti-Pattern)
 * Description: Demonstrates the performance hit when multiple threads modify independent variables 
 *              that happen to share the same CPU Cache Line (usually 64 bytes).
 *              
 *              Solution: Padding (alignas).
 */

#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <new> // for std::hardware_destructive_interference_size

// Struct WITHOUT padding (prone to false sharing)
struct SharedData {
    std::atomic<int> a{0};
    std::atomic<int> b{0};
    std::atomic<int> c{0};
    std::atomic<int> d{0};
};

// Struct WITH padding (aligned to cache line)
struct PaddedData {
    alignas(std::hardware_destructive_interference_size) std::atomic<int> a{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<int> b{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<int> c{0};
    alignas(std::hardware_destructive_interference_size) std::atomic<int> d{0};
};

void work(std::atomic<int>& var) {
    for (int i = 0; i < 10000000; ++i) {
        var.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    SharedData bad;
    PaddedData good;

    auto benchmark = [&](auto& data, const char* name) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::thread t1(work, std::ref(data.a));
        std::thread t2(work, std::ref(data.b));
        std::thread t3(work, std::ref(data.c));
        std::thread t4(work, std::ref(data.d));

        t1.join(); t2.join(); t3.join(); t4.join();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << name << ": " << elapsed.count() << " ms\n";
    };

    std::cout << "--- False Sharing Benchmark ---" << std::endl;
    benchmark(bad, "Without Padding (False Sharing)");
    benchmark(good, "With Padding    (No Sharing)   ");

    return 0;
}
