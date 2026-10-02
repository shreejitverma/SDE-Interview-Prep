#!/opt/aos-venv/bin/python
import random

class VectorClock:
    def __init__(self, clocks=None):
        self.clocks = clocks if clocks else {}
        
    def increment(self, node_id):
        self.clocks[node_id] = self.clocks.get(node_id, 0) + 1
        
    def dominates(self, other):
        # self strictly greater than other?
        # self >= other and self != other
        all_ge = True
        any_gt = False
        all_keys = set(self.clocks.keys()).union(set(other.clocks.keys()))
        for k in all_keys:
            v1 = self.clocks.get(k, 0)
            v2 = other.clocks.get(k, 0)
            if v1 < v2:
                all_ge = False
                break
            if v1 > v2:
                any_gt = True
        return all_ge and any_gt

    def __str__(self):
        return str(self.clocks)

class DynamoNode:
    def __init__(self, node_id):
        self.id = node_id
        self.data = {} # key -> [(value, vector_clock)]

    def write(self, key, value, vclock):
        if key not in self.data:
            self.data[key] = []
            
        current = self.data[key]
        new_versions = []
        dominated = False
        
        for val, vc in current:
            if vc.dominates(vclock):
                dominated = True
                new_versions.append((val, vc))
            elif vclock.dominates(vc):
                pass # vclock replaces vc
            else:
                new_versions.append((val, vc))
                
        if not dominated:
            new_versions.append((value, vclock))
            
        self.data[key] = new_versions

    def read(self, key):
        return self.data.get(key, [])

class DynamoSystem:
    def __init__(self, n, r, w):
        self.N = n
        self.R = r
        self.W = w
        self.nodes = [DynamoNode(i) for i in range(n)]

    def _get_preference_list(self):
        return random.sample(self.nodes, self.N)

    def write(self, key, value, context_vclock, coordinator_idx):
        pref_list = self._get_preference_list()
        coordinator = pref_list[coordinator_idx]
        
        new_vclock = VectorClock(dict(context_vclock.clocks))
        new_vclock.increment(coordinator.id)
        
        success = 0
        for node in pref_list[:self.W]:
            node.write(key, value, new_vclock)
            success += 1
            
        print(f"Write '{value}' handled by Node {coordinator.id}, replicated to {success} nodes. VClock: {new_vclock}")
        return new_vclock

    def read(self, key):
        pref_list = self._get_preference_list()
        results = []
        for node in pref_list[:self.R]:
            for val, vc in node.read(key):
                results.append((val, vc))
                
        final_versions = []
        for val, vc in results:
            dominated = False
            for val2, vc2 in results:
                if vc2.dominates(vc):
                    dominated = True
                    break
            if not dominated:
                if not any(v == val for v, c in final_versions):
                    final_versions.append((val, vc))
                    
        return final_versions

if __name__ == "__main__":
    print("=== Dynamo Quorums and Vector Clocks ===")
    random.seed(123)
    system = DynamoSystem(n=3, r=2, w=2)
    key = "cart"
    
    print("\n[Client] Initial write:")
    vc1 = system.write(key, "item1", VectorClock(), 0)
    
    print("\n[Client] Read:")
    res = system.read(key)
    for val, vc in res:
        print(f"  -> Returned: {val} (Clock: {vc})")
        
    print("\n[Client] Update write:")
    vc2 = system.write(key, "item1,item2", vc1, 0)
    
    print("\n[Client] Concurrent writes (Partition):")
    vc3a = system.write(key, "item1,item2,item3A", vc2, 1)
    vc3b = system.write(key, "item1,item2,item3B", vc2, 2)
    
    print("\n[Client] Read with conflict:")
    res = system.read(key)
    for val, vc in res:
        print(f"  -> Returned: {val} (Clock: {vc})")

