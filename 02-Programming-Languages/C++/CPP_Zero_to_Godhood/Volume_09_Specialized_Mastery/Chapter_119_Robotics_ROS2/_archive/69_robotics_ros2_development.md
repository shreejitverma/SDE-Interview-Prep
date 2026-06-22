# Chapter 69: Robotics & ROS2 Development

# ROBOTICS & ROS2 DEVELOPMENT

Robotics is where Soft Real-Time (Navigation) meets Hard Real-Time (Motor Control).

### 1. ROS2 Architecture & DDS

Robot Operating System 2 (ROS2) runs on top of DDS (Data Distribution Service).
*   **Nodes:** Independent processes.
*   **Topics:** Pub/Sub channels.
*   **Services:** RPC-style calls.

### 2. Zero-Copy Transport (Iceoryx)

Standard ROS2 serialization is slow for large data (LiDAR point clouds, 4K video).
*   **Solution:** Shared Memory.
*   **Mechanism:**
    1.  Publisher requests a memory chunk from shared segment.
    2.  Publisher writes data directly.
    3.  Publisher sends the *offset* (pointer) to Subscriber.
    4.  Subscriber reads directly. **Zero copies.**

### 3. Real-Time Executors

Standard ROS2 executors can suffer from priority inversion.
*   **Callback-group-level Executor:** Prioritize "Safety Stop" topic callbacks over "Camera Logging" callbacks.

### 4. Custom Allocators (`std::pmr`)

In the real-time control loop (1kHz+), heap fragmentation is fatal.
*   **Pattern:** Use `std::pmr::monotonic_buffer_resource` on the stack for message generation.

