import sys

class TransactionLog:
    def __init__(self, name):
        self.name = name
        self.records = []

    def force(self, record):
        self.records.append(record)
        print(f"[{self.name}] FORCED LOG: {record}")

class Node:
    def __init__(self, name):
        self.name = name
        self.log = TransactionLog(name)
        self.subordinates = []
        self.crashed = False

    def add_subordinate(self, node):
        self.subordinates.append(node)

    def receive_vote_request(self, txn_id):
        if self.crashed:
            return "ABORT" # Simulated timeout
        print(f"[{self.name}] Received Vote Request for {txn_id}")
        
        for sub in self.subordinates:
            vote = sub.receive_vote_request(txn_id)
            if vote == "ABORT":
                print(f"[{self.name}] Subordinate {sub.name} voted ABORT")
                return "ABORT"
                
        self.log.force(f"{txn_id}: VOTE COMMIT")
        print(f"[{self.name}] Voted COMMIT for {txn_id}")
        return "COMMIT"

    def receive_end_commit(self, txn_id):
        if self.crashed:
            print(f"[{self.name}] CRASHED, missed End Commit for {txn_id}")
            return
        print(f"[{self.name}] Received End Commit for {txn_id}")
        for sub in self.subordinates:
            sub.receive_end_commit(txn_id)
        self.log.force(f"{txn_id}: FORGET")

class Coordinator(Node):
    def commit_transaction(self, txn_id):
        print(f"\n--- Initiating 2PC for {txn_id} ---")
        abort = False
        for sub in self.subordinates:
            vote = sub.receive_vote_request(txn_id)
            if vote == "ABORT":
                abort = True
                break
                
        if abort:
            print(f"[{self.name}] Transaction {txn_id} ABORTED.")
            self.log.force(f"{txn_id}: ABORT")
            return
            
        self.log.force(f"{txn_id}: GLOBAL COMMIT")
        print(f"[{self.name}] Transaction {txn_id} COMMITTED. Sending End Commit.")
        for sub in self.subordinates:
            sub.receive_end_commit(txn_id)
        self.log.force(f"{txn_id}: FORGET")

def test_normal_commit():
    coord = Coordinator("Coordinator")
    sub1 = Node("Subordinate-1")
    server1 = Node("Server-1")
    sub1.add_subordinate(server1)
    
    server2 = Node("Server-2")
    coord.add_subordinate(sub1)
    coord.add_subordinate(server2)

    coord.commit_transaction("TXN-1")

def test_crash_during_voting():
    coord = Coordinator("Coordinator")
    sub1 = Node("Subordinate-1")
    server1 = Node("Server-1")
    
    server1.crashed = True
    sub1.add_subordinate(server1)
    coord.add_subordinate(sub1)

    coord.commit_transaction("TXN-2")

def test_coordinator_crash_after_commit():
    print("\n--- Initiating 2PC with Coordinator Crash ---")
    coord = Coordinator("Coordinator")
    sub1 = Node("Subordinate-1")
    coord.add_subordinate(sub1)
    
    vote = sub1.receive_vote_request("TXN-3")
    if vote == "COMMIT":
        coord.log.force("TXN-3: GLOBAL COMMIT")
        print(f"[{coord.name}] CRASHES immediately after writing GLOBAL COMMIT record!")
        coord.crashed = True
        
    print("\n--- Recovery Phase ---")
    print(f"[{coord.name}] Recovers, reads log.")
    if "TXN-3: GLOBAL COMMIT" in coord.log.records:
        print(f"[{coord.name}] Found GLOBAL COMMIT for TXN-3. Resending End Commit.")
        coord.crashed = False
        sub1.receive_end_commit("TXN-3")

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "run":
        test_normal_commit()
        test_crash_during_voting()
        test_coordinator_crash_after_commit()
        print("\nAll simulations passed.")
