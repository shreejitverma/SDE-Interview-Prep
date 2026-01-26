/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Producer-Consumer Problem (Concurrency)
 * Description: A thread-safe implementation using std::mutex and std::condition_variable.
 *           This demonstrates handling race conditions and thread synchronization.
 */

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class BoundedBuffer {
private:
    std::queue<int> buffer;
    const size_t capacity;
    std::mutex mtx;
    std::condition_variable not_full;
    std::condition_variable not_empty;
    bool finished = false;

public:
    BoundedBuffer(size_t cap) : capacity(cap) {}

    void produce(int item) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait until buffer is not full
        not_full.wait(lock, [this]() {
            return buffer.size() < capacity; 
        });

        buffer.push(item);
        std::cout << "Produced: " << item << " | Buffer Size: " << buffer.size() << "\n";

        // Notify one waiting consumer
        not_empty.notify_one();
    }

    void consume(int id) {
        std::unique_lock<std::mutex> lock(mtx);

        // Wait until buffer is not empty or production is finished
        not_empty.wait(lock, [this]() { 
            return !buffer.empty() || finished; 
        });

        if (!buffer.empty()) {
            int item = buffer.front();
            buffer.pop();
            std::cout << "Consumer " << id << " consumed: " << item << "\n";
            
            // Notify one waiting producer
            not_full.notify_one();
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            finished = true;
        }
        not_empty.notify_all(); // Wake up all consumers to finish
    }
};

void producer_thread(BoundedBuffer& buffer, int count) {
    for (int i = 0; i < count; ++i) {
        buffer.produce(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate work
    }
    buffer.stop();
}

void consumer_thread(BoundedBuffer& buffer, int id) {
    // In a real app, this would loop until a stop signal
    for (int i = 0; i < 5; ++i) {
        buffer.consume(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
    }
}

int main() {
    BoundedBuffer buffer(5); // Capacity 5

    std::thread p(producer_thread, std::ref(buffer), 10);
    std::thread c1(consumer_thread, std::ref(buffer), 1);
    std::thread c2(consumer_thread, std::ref(buffer), 2);

    p.join();
    c1.join();
    c2.join();

    std::cout << "Processing complete.\n";
    return 0;
}
