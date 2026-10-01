#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_NODES 3
#define NUM_CS_ENTRIES 2

typedef enum {
    MSG_REQUEST,
    MSG_REPLY,
    MSG_RELEASE
} MsgType;

typedef struct {
    MsgType type;
    int src;
    int timestamp;
} Message;

typedef struct {
    Message msgs[100];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Mailbox;

typedef struct {
    int id;
    int ts;
} Request;

typedef struct {
    int id;
    int lamport;
    Mailbox mbox;
    
    Request req_queue[NUM_NODES];
    int req_count;
    
    int replies_received;
    bool requesting;
} Node;

Node nodes[NUM_NODES];
int total_messages = 0;
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

void send_message(int dst, MsgType type, int src, int ts) {
    pthread_mutex_lock(&global_lock);
    total_messages++;
    pthread_mutex_unlock(&global_lock);

    Mailbox* mb = &nodes[dst].mbox;
    pthread_mutex_lock(&mb->lock);
    mb->msgs[mb->tail].type = type;
    mb->msgs[mb->tail].src = src;
    mb->msgs[mb->tail].timestamp = ts;
    mb->tail = (mb->tail + 1) % 100;
    mb->count++;
    pthread_cond_signal(&mb->cond);
    pthread_mutex_unlock(&mb->lock);
}

void broadcast_request(int src, int ts) {
    for (int i = 0; i < NUM_NODES; i++) {
        if (i != src) {
            send_message(i, MSG_REQUEST, src, ts);
        }
    }
}

void broadcast_release(int src, int ts) {
    for (int i = 0; i < NUM_NODES; i++) {
        if (i != src) {
            send_message(i, MSG_RELEASE, src, ts);
        }
    }
}

void add_request(Node* n, int id, int ts) {
    n->req_queue[n->req_count].id = id;
    n->req_queue[n->req_count].ts = ts;
    n->req_count++;
    
    // Sort by ts, then by id
    for (int i = 0; i < n->req_count - 1; i++) {
        for (int j = 0; j < n->req_count - i - 1; j++) {
            bool swap = false;
            if (n->req_queue[j].ts > n->req_queue[j+1].ts) swap = true;
            else if (n->req_queue[j].ts == n->req_queue[j+1].ts && n->req_queue[j].id > n->req_queue[j+1].id) swap = true;
            
            if (swap) {
                Request temp = n->req_queue[j];
                n->req_queue[j] = n->req_queue[j+1];
                n->req_queue[j+1] = temp;
            }
        }
    }
}

void remove_request(Node* n, int id) {
    int idx = -1;
    for (int i = 0; i < n->req_count; i++) {
        if (n->req_queue[i].id == id) {
            idx = i;
            break;
        }
    }
    if (idx != -1) {
        for (int i = idx; i < n->req_count - 1; i++) {
            n->req_queue[i] = n->req_queue[i+1];
        }
        n->req_count--;
    }
}

bool can_enter_cs(Node* n) {
    if (n->req_count == 0) return false;
    if (n->req_queue[0].id == n->id && n->replies_received == NUM_NODES - 1) {
        return true;
    }
    return false;
}

void* node_run(void* arg) {
    int id = *(int*)arg;
    Node* n = &nodes[id];
    
    for (int iter = 0; iter < NUM_CS_ENTRIES; iter++) {
        usleep(rand() % 10000); // random delay before requesting
        
        // 1. Request CS
        n->lamport++;
        int req_ts = n->lamport;
        n->requesting = true;
        n->replies_received = 0;
        add_request(n, n->id, req_ts);
        broadcast_request(n->id, req_ts);
        
        // 2. Wait for replies and top of queue
        while (1) {
            Mailbox* mb = &n->mbox;
            pthread_mutex_lock(&mb->lock);
            while (mb->count == 0) {
                pthread_cond_wait(&mb->cond, &mb->lock);
            }
            Message m = mb->msgs[mb->head];
            mb->head = (mb->head + 1) % 100;
            mb->count--;
            pthread_mutex_unlock(&mb->lock);
            
            // update lamport
            if (m.timestamp > n->lamport) n->lamport = m.timestamp;
            n->lamport++;
            
            if (m.type == MSG_REQUEST) {
                add_request(n, m.src, m.timestamp);
                send_message(m.src, MSG_REPLY, n->id, n->lamport);
            } else if (m.type == MSG_REPLY) {
                n->replies_received++;
            } else if (m.type == MSG_RELEASE) {
                remove_request(n, m.src);
            }
            
            if (n->requesting && can_enter_cs(n)) {
                break;
            }
        }
        
        // 3. Critical Section
        printf("Node %d ENTERING CS (Iter %d) with Lamport %d\n", n->id, iter, n->lamport);
        usleep(1000);
        printf("Node %d LEAVING CS (Iter %d)\n", n->id, iter);
        
        // 4. Release CS
        n->requesting = false;
        remove_request(n, n->id);
        n->lamport++;
        broadcast_release(n->id, n->lamport);
    }
    
    // We need to keep processing messages so others aren't deadlocked waiting for replies.
    // In a real system, nodes run forever. Here we just loop a bit.
    for (int i=0; i<50; i++) {
        Mailbox* mb = &n->mbox;
        pthread_mutex_lock(&mb->lock);
        if (mb->count > 0) {
            Message m = mb->msgs[mb->head];
            mb->head = (mb->head + 1) % 100;
            mb->count--;
            pthread_mutex_unlock(&mb->lock);
            
            if (m.timestamp > n->lamport) n->lamport = m.timestamp;
            n->lamport++;
            
            if (m.type == MSG_REQUEST) {
                add_request(n, m.src, m.timestamp);
                send_message(m.src, MSG_REPLY, n->id, n->lamport);
            } else if (m.type == MSG_RELEASE) {
                remove_request(n, m.src);
            }
        } else {
            pthread_mutex_unlock(&mb->lock);
            usleep(1000);
        }
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_NODES];
    int ids[NUM_NODES];
    
    for (int i = 0; i < NUM_NODES; i++) {
        nodes[i].id = i;
        nodes[i].lamport = 0;
        nodes[i].req_count = 0;
        nodes[i].requesting = false;
        nodes[i].mbox.head = 0;
        nodes[i].mbox.tail = 0;
        nodes[i].mbox.count = 0;
        pthread_mutex_init(&nodes[i].mbox.lock, NULL);
        pthread_cond_init(&nodes[i].mbox.cond, NULL);
    }
    
    for (int i = 0; i < NUM_NODES; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, node_run, &ids[i]);
    }
    
    for (int i = 0; i < NUM_NODES; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Total Messages EXCHANGED: %d\n", total_messages);
    int expected = NUM_NODES * NUM_CS_ENTRIES * 3 * (NUM_NODES - 1);
    printf("Expected Messages (3*(N-1) per CS): %d\n", expected);
    
    return 0;
}
