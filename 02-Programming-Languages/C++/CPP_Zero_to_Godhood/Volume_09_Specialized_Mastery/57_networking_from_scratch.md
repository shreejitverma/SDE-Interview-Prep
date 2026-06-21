# Chapter 57: Networking From Scratch

# NETWORKING FROM SCRATCH

Understanding `asio` requires understanding BSD Sockets.

### 28.1 Berkeley Sockets API

The foundation of the Internet.

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    int new_socket = accept(server_fd, nullptr, nullptr);
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);

    // Send HTTP response
    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello!";
    write(new_socket, hello, strlen(hello));

    close(new_socket);
    close(server_fd);
    return 0;
}
```

### 28.2 Non-Blocking I/O & Epoll (Linux)

How Nginx/Node.js handle 10k connections.

```cpp
// 1. Create epoll instance
int epoll_fd = epoll_create1(0);

// 2. Add server socket
epoll_event event;
event.events = EPOLLIN; // Read available
event.data.fd = server_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

// 3. Event Loop
while (true) {
    epoll_event events[10];
    int event_count = epoll_wait(epoll_fd, events, 10, -1);
    for (int i = 0; i < event_count; i++) {
        if (events[i].data.fd == server_fd) {
            // Accept new connection...
        } else {
            // Read data...
        }
    }
}
```

