#!/opt/aos-venv/bin/python
import hashlib
import random

M = 16 # 16-bit identifier space

class Node:
    def __init__(self, node_id):
        self.id = node_id
        self.finger = []
        
    def init_finger_table(self, active_nodes):
        self.finger = []
        for i in range(M):
            start = (self.id + 2**i) % (2**M)
            # Find successor of start
            succ = None
            for n_id in active_nodes:
                if n_id >= start:
                    succ = n_id
                    break
            if succ is None:
                succ = active_nodes[0]
            self.finger.append(succ)

    def find_successor(self, key_id, active_nodes_map, hops):
        if self.id < self.finger[0]:
            in_interval = self.id < key_id <= self.finger[0]
        else:
            in_interval = self.id < key_id or key_id <= self.finger[0]
            
        if in_interval:
            return self.finger[0], hops + 1
        else:
            # Find closest preceding node
            closest = self.id
            for i in range(M - 1, -1, -1):
                f_id = self.finger[i]
                if self.id < key_id:
                    if self.id < f_id < key_id:
                        closest = f_id
                        break
                else:
                    if self.id < f_id or f_id < key_id:
                        closest = f_id
                        break
            if closest == self.id: # fallback to successor
                closest = self.finger[0]
            return active_nodes_map[closest].find_successor(key_id, active_nodes_map, hops + 1)

if __name__ == "__main__":
    print(f"=== Chord Routing Simulation (m={M}) ===")
    random.seed(42)
    num_nodes = 100
    # Generate random unique nodes
    active_nodes = sorted(random.sample(range(2**M), num_nodes))
    
    nodes_map = {n_id: Node(n_id) for n_id in active_nodes}
    for n_id in active_nodes:
        nodes_map[n_id].init_finger_table(active_nodes)
        
    # Simulate lookups
    num_lookups = 1000
    total_hops = 0
    for _ in range(num_lookups):
        start_node_id = random.choice(active_nodes)
        key_id = random.randint(0, 2**M - 1)
        _, hops = nodes_map[start_node_id].find_successor(key_id, nodes_map, 0)
        total_hops += hops
        
    print(f"Nodes: {num_nodes}, Lookups: {num_lookups}")
    print(f"Average hops per lookup: {total_hops / num_lookups:.2f}")
    expected_hops = (M / 2)
    print(f"Expected hops ~ O(log N): ~ {M/2 if num_nodes == 2**M else 'variable, < ' + str(M)}")

