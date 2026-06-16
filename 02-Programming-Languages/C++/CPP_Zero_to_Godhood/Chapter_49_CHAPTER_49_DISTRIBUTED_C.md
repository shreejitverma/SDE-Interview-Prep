# CHAPTER 49: DISTRIBUTED C


# DISTRIBUTED C++

Moving beyond a single process: Networking, RPC, and Consensus.

### 1. Serialization: The "Box and Label" Problem

When you send an object (like a `User` class) over the network, you can't just send the memory address. The address `0x123` on your computer doesn't mean anything to another computer across the world.

Instead, you have to **Serialize** it. This is like taking a LEGO castle, breaking it down into individual bricks, putting them in a numbered box with instructions, and shipping it. The receiver then **Deserializes** itrebuilding the castle brick-by-brick.

#### 1.1 Serialization (Binary Protocols)
Efficiently packing data for network transmission.

```cpp
#include <vector>
#include <cstring>
#include <string>

// Simple Binary Serializer
class Buffer {
    std::vector<uint8_t> data;
public:
    template<typename T>
    void write(const T& val) {
        static_assert(std::is_trivially_copyable_v<T>);
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&val);
        data.insert(data.end(), ptr, ptr + sizeof(T));
    }

    void write_string(const std::string& s) {
        write<uint32_t>(s.size());
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s.data());
        data.insert(data.end(), ptr, ptr + s.size());
    }

    const uint8_t* begin() const { return data.data(); }
    size_t size() const { return data.size(); }
};
```

### 16.2 RPC (Remote Procedure Call) Concept
Calling a function on another machine.

**Stub Interface:**
```cpp
// User Code
// auto result = service.Add(5, 3);

// Generated Stub
int Add(int a, int b) {
    Buffer buf;
    buf.write(101); // Function ID for 'Add'
    buf.write(a);
    buf.write(b);
    return network.send_and_wait(buf); // Blocks
}
```

### 16.3 Consensus (Raft Basics)
Distributed systems need to agree on state.

**Raft State Machine:**
```cpp
enum class State { Follower, Candidate, Leader };

struct Node {
    State state = State::Follower;
    int current_term = 0;
    int voted_for = -1;

    void on_timeout() {
        if (state == State::Follower) {
            state = State::Candidate;
            current_term++;
            voted_for = my_id;
            request_votes();
        }
    }
};
```
