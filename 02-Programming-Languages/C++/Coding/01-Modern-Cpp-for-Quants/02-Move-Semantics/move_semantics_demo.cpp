/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Move Semantics (std::move)
 * Description: Optimizing performance by transferring resources instead of copying deep data structures.
 *           Crucial for low-latency HFT applications where copying large objects (like order books) is expensive.
 */

#include <iostream>
#include <vector>
#include <string>

class LargeDataSet {
public:
    std::vector<int> data;

    // Constructor
    LargeDataSet(size_t size) {
        data.resize(size);
        std::cout << "Constructed dataset of size " << size << "\n";
    }

    // Copy Constructor (Expensive)
    LargeDataSet(const LargeDataSet& other) : data(other.data) {
        std::cout << "COPY Constructor called (Expensive!)\n";
    }

    // Move Constructor (Cheap)
    LargeDataSet(LargeDataSet&& other) noexcept : data(std::move(other.data)) {
        std::cout << "MOVE Constructor called (Cheap!)\n";
    }
};

int main() {
    LargeDataSet set1(1000000); // 1 Million integers

    std::cout << "--- Copying ---\n";
    LargeDataSet set2 = set1; // Deep copy

    std::cout << "--- Moving ---\n";
    LargeDataSet set3 = std::move(set1); // Move: set1 is now empty, set3 owns the data

    std::cout << "Set1 size: " << set1.data.size() << " (Should be 0)\n";
    std::cout << "Set3 size: " << set3.data.size() << " (Should be 1M)\n";

    return 0;
}
