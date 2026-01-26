/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Write-Ahead Logging (WAL)
 * Description: A mechanism to ensure database durability (ACID).
 *           Operations are appended to a log file before being applied to the in-memory state.
 *           If the system crashes, we replay the log to recover the state.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

// Represents a Database Operation
struct LogEntry {
    std::string key;
    std::string value;
};

class Database {
    std::map<std::string, std::string> memTable; // In-memory Storage
    std::ofstream logFile;
    const std::string LOG_FILENAME = "wal.log";

public:
    Database() {
        recover(); // Replay WAL on startup
        
        // Open log file in append mode
        logFile.open(LOG_FILENAME, std::ios::app);
    }

    void put(const std::string& key, const std::string& value) {
        // 1. Write to Disk (WAL) - The "Durability" Step
        logFile << key << "," << value << "\n";
        logFile.flush(); // Ensure it hits the OS buffer

        // 2. Update Memory
        memTable[key] = value;
        std::cout << "Stored: " << key << " = " << value << "\n";
    }

    std::string get(const std::string& key) {
        if (memTable.count(key)) {
            return memTable[key];
        }
        return "NULL";
    }

private:
    void recover() {
        std::ifstream inFile(LOG_FILENAME);
        if (!inFile.is_open()) return;

        std::cout << "--- Recovering from WAL ---\n";
        std::string line;
        while (std::getline(inFile, line)) {
            std::stringstream ss(line);
            std::string key, value;
            
            // Simple parsing (CSV)
            if (std::getline(ss, key, ',') && std::getline(ss, value)) {
                memTable[key] = value;
                std::cout << "Replayed: " << key << " = " << value << "\n";
            }
        }
        std::cout << "--- Recovery Complete ---\n";
    }
};

int main() {
    Database db;

    db.put("user:1", "Alice");
    db.put("user:2", "Bob");

    // If you run this program again, "Alice" and "Bob" will be recovered from wal.log
    
    return 0;
}
