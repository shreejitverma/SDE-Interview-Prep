/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: LSM Tree (Log-Structured Merge Tree) Component - MemTable
 * Description: The write-optimized structure used in RocksDB and Cassandra.
 *           Writes go to MemTable (RAM, Sorted Map). When full, flushed to SSTable (Disk).
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>

class MemTable {
    // Sorted Map allows range queries and ordered flush
    std::map<std::string, std::string> table;
    size_t size_bytes = 0;
    const size_t CAPACITY = 1024; // Small for demo

public:
    void put(const std::string& key, const std::string& value) {
        // WAL (Write Ahead Log) should happen here for durability
        
        size_bytes += key.size() + value.size();
        table[key] = value;

        if (size_bytes >= CAPACITY) {
            flush_to_disk();
        }
    }

    std::string get(const std::string& key) {
        if (table.count(key)) {
            return table[key];
        }
        return ""; // In real LSM, check Bloom Filter -> SSTables on Disk
    }

    void flush_to_disk() {
        std::cout << "--- FLUSHING MEMTABLE TO SSTABLE (DISK) ---\\n";
        for (const auto& pair : table) {
            std::cout << "[Disk Write] " << pair.first << ": " << pair.second << "\\n";
        }
        table.clear();
        size_bytes = 0;
    }
};

int main() {
    MemTable memtable;

    // High throughput writes
    memtable.put("user:1", "{name: Alice}");
    memtable.put("user:2", "{name: Bob}");
    memtable.put("user:3", "{name: Charlie}");

    // Read latest data (from RAM)
    std::cout << "Read user:2 -> " << memtable.get("user:2") << "\\n";

    // Simulate Fill
    for (int i = 0; i < 20; ++i) {
        memtable.put("log:" + std::to_string(i), "data payload...");
    }

    return 0;
}
