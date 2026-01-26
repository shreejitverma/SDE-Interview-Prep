# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Consistent Hashing
# Description: Implements a Consistent Hash Ring with Virtual Nodes.
#              Used by Cassandra, DynamoDB, and Load Balancers to distribute keys evenly
#              and minimize data movement when nodes are added/removed.

import hashlib
import bisect

class ConsistentHashRing:
    def __init__(self, nodes=None, replicas=3):
        self.replicas = replicas
        self.ring = dict()  # Map: Hash -> Node Name
        self.sorted_keys = [] # Sorted Hashes for Binary Search

        if nodes:
            for node in nodes:
                self.add_node(node)

    def _hash(self, key):
        """Returns a hash integer for the key using MD5."""
        return int(hashlib.md5(key.encode('utf-8')).hexdigest(), 16)

    def add_node(self, node):
        """Adds a physical node (and its virtual replicas) to the ring."""
        for i in range(self.replicas):
            virtual_node_key = f"{node}:{i}"
            key_hash = self._hash(virtual_node_key)
            self.ring[key_hash] = node
            bisect.insort(self.sorted_keys, key_hash)
        print(f"Added Node: {node}")

    def remove_node(self, node):
        """Removes a physical node from the ring."""
        for i in range(self.replicas):
            virtual_node_key = f"{node}:{i}"
            key_hash = self._hash(virtual_node_key)
            del self.ring[key_hash]
            self.sorted_keys.remove(key_hash)
        print(f"Removed Node: {node}")

    def get_node(self, key):
        """
        Returns the node responsible for the given key.
        Performs Binary Search (upper_bound) on the sorted ring.
        """
        if not self.ring:
            return None

        key_hash = self._hash(key)
        # Find the first node clockwise (idx)
        idx = bisect.bisect_right(self.sorted_keys, key_hash)

        # Wrap around to 0 if at the end of the ring
        if idx == len(self.sorted_keys):
            idx = 0

        return self.ring[self.sorted_keys[idx]]

if __name__ == "__main__":
    ch = ConsistentHashRing(nodes=["Server-A", "Server-B", "Server-C"])

    # Test Distribution
    keys = ["User1", "User2", "User3", "User4", "User5"]
    
    print("\n--- Initial Distribution ---")
    for k in keys:
        print(f"{k} -> {ch.get_node(k)}")

    # Add a Node (Rebalancing)
    print("\n--- Adding Server-D ---")
    ch.add_node("Server-D")
    
    for k in keys:
        print(f"{k} -> {ch.get_node(k)}")
    
    # Remove a Node (Failover)
    print("\n--- Removing Server-A ---")
    ch.remove_node("Server-A")
    
    for k in keys:
        print(f"{k} -> {ch.get_node(k)}")
