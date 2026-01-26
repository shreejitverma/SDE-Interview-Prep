/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Singleton Pattern (Thread-Safe)
 * Description: Implementation of Meyers' Singleton. 
 *           In C++11 and later, static local variable initialization is thread-safe.
 */

#include <iostream>
#include <thread>
#include <vector>

class DatabaseConnection {
public:
    // Delete copy constructor and assignment operator to prevent copies
    DatabaseConnection(const DatabaseConnection&) = delete;
    void operator=(const DatabaseConnection&) = delete;

    // Static method to get the single instance
    static DatabaseConnection& getInstance() {
        // Guaranteed to be thread-safe in C++11+
        static DatabaseConnection instance;
        return instance;
    }

    void query(const std::string& sql) {
        std::cout << "Executing query: " << sql << " on Instance: " << this << "\n";
    }

private:
    // Private constructor to prevent direct instantiation
    DatabaseConnection() {
        std::cout << "Database Connection Initialized.\n";
    }
};

void worker_thread() {
    DatabaseConnection& db = DatabaseConnection::getInstance();
    db.query("SELECT * FROM users");
}

int main() {
    std::cout << "Main: Starting threads...\n";

    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i) {
        threads.emplace_back(worker_thread);
    }

    for(auto& t : threads) {
        t.join();
    }

    return 0;
}
