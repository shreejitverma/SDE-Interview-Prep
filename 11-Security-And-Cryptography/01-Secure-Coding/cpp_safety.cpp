/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Secure Coding in C++
 * Description: Examples of avoiding Buffer Overflows, Integer Overflows, and using modern C++ for safety.
 */

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <stdexcept>

// 1. Preventing Integer Overflow
// Use checked arithmetic or checks before operation
int safe_multiply(int a, int b) {
    if (a > 0 && b > 0 && a > std::numeric_limits<int>::max() / b) {
        throw std::overflow_error("Integer Overflow detected");
    }
    return a * b;
}

// 2. Preventing Buffer Overflow
// AVOID C-style arrays (char buffer[10]) and strcpy.
// USE std::string and std::vector which manage memory automatically.
void safe_string_copy(const std::string& input) {
    std::vector<char> buffer(10); // Dynamic but small capacity
    
    // BAD: strcpy(buffer.data(), input.c_str()); // Crushes memory if input > 10 chars
    
    // GOOD: Explicit bounds check or use std::string assignment
    if (input.length() >= buffer.size()) {
        std::cout << "[Security] Input too long! Truncating.\n";
    }
    
    // std::copy is safer with bounds
    size_t length = std::min(input.length(), buffer.size() - 1);
    std::copy(input.begin(), input.begin() + length, buffer.begin());
    buffer[length] = '\0'; // Null terminate
    
    std::cout << "Buffer content: " << buffer.data() << "\n";
}

int main() {
    try {
        std::cout << "Safe Multiply: " << safe_multiply(100, 50) << "\n";
        // safe_multiply(2000000000, 2); // Throws exception
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
    }

    std::string malicious_input = "ThisStringIsWayTooLongForTheBuffer";
    safe_string_copy(malicious_input);

    return 0;
}
