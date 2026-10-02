#!/opt/aos-venv/bin/python
import hashlib
import statistics
import sys
from collections import defaultdict

def hash_key(key: str) -> int:
    return int(hashlib.md5(key.encode()).hexdigest(), 16)

def simulate(num_nodes, vnodes_per_node, num_keys):
    ring = {}
    for i in range(num_nodes):
        node_name = f"Node-{i}"
        for v in range(vnodes_per_node):
            vnode_name = f"{node_name}-V{v}"
            h = hash_key(vnode_name)
            ring[h] = node_name
            
    sorted_hashes = sorted(ring.keys())
    
    node_counts = defaultdict(int)
    for i in range(num_nodes):
        node_counts[f"Node-{i}"] = 0
        
    for k in range(num_keys):
        h = hash_key(f"key-{k}")
        # Find next node
        assigned_node = None
        for r_hash in sorted_hashes:
            if h <= r_hash:
                assigned_node = ring[r_hash]
                break
        if assigned_node is None:
            assigned_node = ring[sorted_hashes[0]]
            
        node_counts[assigned_node] += 1
        
    counts = list(node_counts.values())
    avg = statistics.mean(counts)
    stddev = statistics.stdev(counts) if len(counts) > 1 else 0
    print(f"Nodes: {num_nodes}, VNodes/Node: {vnodes_per_node}, Keys: {num_keys}")
    print(f"Mean keys/node: {avg:.1f}, StdDev: {stddev:.1f} (CV: {stddev/avg*100:.1f}%)")

if __name__ == "__main__":
    print("=== Consistent Hashing Load Balance ===")
    simulate(num_nodes=10, vnodes_per_node=1, num_keys=100000)
    simulate(num_nodes=10, vnodes_per_node=5, num_keys=100000)
    simulate(num_nodes=10, vnodes_per_node=100, num_keys=100000)
