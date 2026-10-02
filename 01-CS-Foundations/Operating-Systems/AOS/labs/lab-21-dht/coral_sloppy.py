#!/opt/aos-venv/bin/python
import random

class CoralNode:
    def __init__(self, node_id, capacity=3):
        self.id = node_id
        self.store = {}
        self.capacity = capacity
        
    def is_full(self, key):
        return len(self.store.get(key, [])) >= self.capacity

    def put(self, key, value):
        if key not in self.store:
            self.store[key] = []
        if len(self.store[key]) < self.capacity:
            self.store[key].append(value)
            return True
        return False

# Simple tree network simulation
# 1 root (closest to key), 2 children, 4 grandchildren, 8 leaves.
# We will simulate routing from a random leaf to the root.

def build_tree():
    nodes = {}
    for i in range(1, 16):
        nodes[i] = CoralNode(i, capacity=2)
    return nodes

def route_to_root(start_id):
    path = []
    curr = start_id
    while curr > 0:
        path.append(curr)
        curr = curr // 2
    return path

def insert_value(nodes, start_node, key, value):
    path = route_to_root(start_node)
    
    # Forward routing: find closest node that is not full
    # Actually Coral checks from start_node towards root.
    # If a node is full, we stop and insert at the previous node.
    last_non_full = None
    for node_id in path:
        if nodes[node_id].is_full(key):
            break
        last_non_full = node_id
        
    if last_non_full is not None:
        nodes[last_non_full].put(key, value)
        return last_non_full
    else:
        # even the start node is full!
        return None

if __name__ == "__main__":
    print("=== Coral Sloppy DHT Spill Simulation ===")
    nodes = build_tree()
    key = "hot-key"
    
    # 8 leaf nodes are 8..15
    leaves = list(range(8, 16))
    
    for i in range(10):
        start = random.choice(leaves)
        stored_at = insert_value(nodes, start, key, f"val-{i}")
        if stored_at:
            print(f"Insert {i:2d} from leaf {start:2d} -> stored at node {stored_at:2d}")
        else:
            print(f"Insert {i:2d} from leaf {start:2d} -> DROPPED (path full)")
            
    print("\nStorage state (key='hot-key'):")
    for i in range(1, 16):
        if key in nodes[i].store:
            print(f"Node {i:2d}: {nodes[i].store[key]}")

