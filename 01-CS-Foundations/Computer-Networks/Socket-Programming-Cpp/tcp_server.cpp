/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: TCP Echo Server (C++ Sockets)
 * Description: A basic synchronous TCP server that accepts connections and echoes messages.
 *           In HFT, you would use non-blocking sockets with epoll (Linux) or kqueue (Mac) 
 *           for handling thousands of connections (Kernel Bypass is the next level).
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create Socket File Descriptor
    // AF_INET = IPv4, SOCK_STREAM = TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    // 2. Attach socket to port 8080 (Optional: prevent "Address already in use" error)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces (0.0.0.0)
    address.sin_port = htons(PORT);

    // 3. Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }

    // 4. Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    // 5. Accept Connection (Blocking)
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        return -1;
    }

    // 6. Read & Echo
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    std::cout << "Received: " << buffer << "\n";
    
    const char* msg = "Hello from C++ Server";
    send(new_socket, msg, strlen(msg), 0);
    std::cout << "Response sent.\n";

    // Close sockets
    close(new_socket);
    close(server_fd);

    return 0;
}
