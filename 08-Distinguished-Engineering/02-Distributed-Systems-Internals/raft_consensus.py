# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Raft Consensus Algorithm (Core Logic)
# Description: Demonstrates Leader Election and Log Replication logic.
#              Raft is used in etcd, Kubernetes, and CockroachDB.

import random
import time
from enum import Enum

class State(Enum):
    FOLLOWER = 1
    CANDIDATE = 2
    LEADER = 3

class Node:
    def __init__(self, node_id, cluster_size):
        self.node_id = node_id
        self.cluster_size = cluster_size
        self.state = State.FOLLOWER
        self.term = 0
        self.voted_for = None
        self.log = [] # Entries: {term, command}
        self.votes_received = 0
        
        # Leader State
        self.next_index = {}  # Index of next log entry to send to each peer
        self.match_index = {} # Index of highest log entry replicated on server

    def start_election(self):
        self.state = State.CANDIDATE
        self.term += 1
        self.voted_for = self.node_id
        self.votes_received = 1
        print(f"Node {self.node_id} starting election for Term {self.term}")
        
        # In real impl, send RequestVote RPCs to all peers here
        # self.send_request_vote(...)

    def receive_request_vote(self, candidate_id, candidate_term, last_log_idx, last_log_term):
        """
        Called when this node receives a vote request from a candidate.
        """
        if candidate_term > self.term:
            self.term = candidate_term
            self.state = State.FOLLOWER
            self.voted_for = None
        
        # 1. Reject if term is old
        if candidate_term < self.term:
            return False

        # 2. Grant vote if we haven't voted yet AND candidate's log is up-to-date
        my_last_idx = len(self.log) - 1
        my_last_term = self.log[-1]['term'] if self.log else 0

        log_is_fresh = (last_log_term > my_last_term) or \
                       (last_log_term == my_last_term and last_log_idx >= my_last_idx)

        if (self.voted_for is None or self.voted_for == candidate_id) and log_is_fresh:
            self.voted_for = candidate_id
            print(f"Node {self.node_id} voted for {candidate_id}")
            return True
        
        return False

    def receive_append_entries(self, leader_id, term, entries):
        """
        Called when receiving log entries from Leader (Heartbeat).
        """
        if term >= self.term:
            self.term = term
            self.state = State.FOLLOWER
            print(f"Node {self.node_id} recognized Leader {leader_id}")
            
            # Append entries logic (Simplified)
            if entries:
                self.log.extend(entries)
                print(f"Node {self.node_id} appended entries: {entries}")
            return True
        
        return False

# Simulation
if __name__ == "__main__":
    n1 = Node(1, 3)
    n2 = Node(2, 3)
    n3 = Node(3, 3)
    
    # 1. Node 1 starts election
    n1.start_election()
    
    # 2. Node 2 and 3 vote for Node 1
    if n2.receive_request_vote(n1.node_id, n1.term, -1, 0):
        n1.votes_received += 1
    if n3.receive_request_vote(n1.node_id, n1.term, -1, 0):
        n1.votes_received += 1
        
    # 3. Node 1 becomes Leader
    if n1.votes_received > 3 / 2:
        n1.state = State.LEADER
        print(f"Node 1 is LEADER. Sending Heartbeats...")
        
        # 4. Leader sends log entry
        entries = [{'term': 1, 'cmd': 'SET X=5'}]
        n2.receive_append_entries(n1.node_id, n1.term, entries)
        n3.receive_append_entries(n1.node_id, n1.term, entries)
