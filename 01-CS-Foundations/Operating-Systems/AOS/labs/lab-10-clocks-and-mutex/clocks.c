#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NODES 3

typedef struct {
    int lamport;
    int vector[MAX_NODES];
    double physical;
    double drift_rate;
} Node;

Node nodes[MAX_NODES];

void init_nodes() {
    for (int i = 0; i < MAX_NODES; i++) {
        nodes[i].lamport = 0;
        for (int j = 0; j < MAX_NODES; j++) {
            nodes[i].vector[j] = 0;
        }
        nodes[i].physical = 0.0;
        nodes[i].drift_rate = 0.05 * i; // Node 0: 0%, Node 1: 5%, Node 2: 10% drift
    }
}

void print_state(int node_id, const char* event_name) {
    printf("Node %d %-10s | Lamport: %2d | Vector: [%d, %d, %d] | Physical (drifted): %5.2f\n",
           node_id, event_name, nodes[node_id].lamport,
           nodes[node_id].vector[0], nodes[node_id].vector[1], nodes[node_id].vector[2],
           nodes[node_id].physical);
}

void local_event(int node_id, double real_time) {
    nodes[node_id].lamport += 1;
    nodes[node_id].vector[node_id] += 1;
    nodes[node_id].physical = real_time * (1.0 + nodes[node_id].drift_rate);
    print_state(node_id, "LOCAL");
}

void send_event(int src, int dst, double real_time, int* out_lamport, int* out_vector) {
    (void)dst; // Suppress unused warning
    nodes[src].lamport += 1;
    nodes[src].vector[src] += 1;
    nodes[src].physical = real_time * (1.0 + nodes[src].drift_rate);
    print_state(src, "SEND");
    
    *out_lamport = nodes[src].lamport;
    for (int i = 0; i < MAX_NODES; i++) {
        out_vector[i] = nodes[src].vector[i];
    }
}

void recv_event(int dst, int src_lamport, int* src_vector, double real_time) {
    nodes[dst].lamport = (nodes[dst].lamport > src_lamport ? nodes[dst].lamport : src_lamport) + 1;
    nodes[dst].vector[dst] += 1;
    for (int i = 0; i < MAX_NODES; i++) {
        if (src_vector[i] > nodes[dst].vector[i]) {
            nodes[dst].vector[i] = src_vector[i];
        }
    }
    nodes[dst].physical = real_time * (1.0 + nodes[dst].drift_rate);
    print_state(dst, "RECV");
}

int main() {
    init_nodes();
    printf("--- Initial State ---\n");
    for (int i = 0; i < MAX_NODES; i++) print_state(i, "INIT");
    printf("\n--- Simulation Trace ---\n");
    
    int msg_lamport;
    int msg_vector[MAX_NODES];
    
    local_event(0, 1.0);
    send_event(0, 1, 2.0, &msg_lamport, msg_vector);
    recv_event(1, msg_lamport, msg_vector, 3.0);
    send_event(1, 2, 4.0, &msg_lamport, msg_vector);
    recv_event(2, msg_lamport, msg_vector, 5.0);
    local_event(2, 6.0);
    send_event(0, 2, 7.0, &msg_lamport, msg_vector);
    recv_event(2, msg_lamport, msg_vector, 8.0);
    
    return 0;
}
