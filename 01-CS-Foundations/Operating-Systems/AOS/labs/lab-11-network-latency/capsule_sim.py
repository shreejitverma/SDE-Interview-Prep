import hashlib

class ActiveNode:
    def __init__(self, name):
        self.name = name
        self.soft_store = {}  # type_hash -> code block
        self.neighbors = {}   # name -> ActiveNode

    def connect(self, other):
        self.neighbors[other.name] = other
        other.neighbors[self.name] = self

    def process_capsule(self, capsule):
        type_hash = capsule['type_hash']
        prev_node_name = capsule['previous_node']
        print(f"[{self.name}] Received capsule with hash: {type_hash[:8]}... from {prev_node_name}")
        
        if type_hash in self.soft_store:
            print(f"[{self.name}] Code found in soft store. Executing...")
            self.execute(type_hash, capsule)
        else:
            print(f"[{self.name}] Unknown capsule type. Suspending and requesting code from {prev_node_name}")
            self.request_code(prev_node_name, type_hash, capsule)
            
    def request_code(self, target_name, type_hash, pending_capsule):
        if target_name in self.neighbors:
            # Simulate request to upstream node
            print(f"[{self.name}] -> [{target_name}]: Requesting code for {type_hash[:8]}...")
            code = self.neighbors[target_name].handle_code_request(type_hash)
            if code:
                # Verify MD5
                computed_hash = hashlib.md5(code.encode('utf-8')).hexdigest()
                if computed_hash == type_hash:
                    print(f"[{self.name}] Received and verified code. Storing and resuming...")
                    self.soft_store[type_hash] = code
                    self.execute(type_hash, pending_capsule)
                else:
                    print(f"[{self.name}] Security error: Code hash mismatch! Dropping capsule.")
            else:
                print(f"[{self.name}] Upstream node did not have the code. Dropping capsule.")
        else:
            print(f"[{self.name}] Error: Unknown previous node. Dropping.")

    def handle_code_request(self, type_hash):
        print(f"[{self.name}] Received code request for {type_hash[:8]}...")
        if type_hash in self.soft_store:
            return self.soft_store[type_hash]
        return None

    def execute(self, type_hash, capsule):
        code = self.soft_store[type_hash]
        print(f"[{self.name}] Executing logic: {code} | Payload: {capsule['payload']}")
        # In a real system, the code would decide where to forward it next.
        # We just print a success message here.
        print(f"[{self.name}] Capsule processing complete.\n")


def run_simulation():
    node_a = ActiveNode("NodeA")
    node_b = ActiveNode("NodeB")
    
    node_a.connect(node_b)
    
    # Node A is the sender/upstream node, so it has the code initially.
    code_logic = "FORWARD_TO_DESTINATION"
    type_hash = hashlib.md5(code_logic.encode('utf-8')).hexdigest()
    node_a.soft_store[type_hash] = code_logic
    
    capsule = {
        'type_hash': type_hash,
        'previous_node': "NodeA",
        'payload': "Hello Active Network!"
    }
    
    print("=== Simulation 1: Unknown Capsule Arrives at NodeB ===")
    node_b.process_capsule(capsule)
    
    print("=== Simulation 2: Subsequent Capsule Arrives at NodeB ===")
    capsule2 = {
        'type_hash': type_hash,
        'previous_node': "NodeA",
        'payload': "Second message"
    }
    node_b.process_capsule(capsule2)


if __name__ == "__main__":
    run_simulation()
