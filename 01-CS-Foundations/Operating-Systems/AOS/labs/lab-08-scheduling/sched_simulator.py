#!/opt/aos-venv/bin/python
import argparse
from collections import deque

import random

class Task:
    def __init__(self, task_id, length):
        self.task_id = task_id
        self.length = length
        self.remaining = length
        self.last_cpu = -1

def run_simulation(policy, num_cpus, num_tasks, quantum):
    random.seed(42)
    tasks = [Task(i, random.randint(10, 100)) for i in range(num_tasks)]
    time = 0
    cpus = [None] * num_cpus
    cpu_quanta = [0] * num_cpus
    cpu_history = [[] for _ in range(num_cpus)]
    migrations = 0
    
    if policy == "fixed":
        queues = [deque() for _ in range(num_cpus)]
        for t in tasks:
            queues[t.task_id % num_cpus].append(t)
    else:
        ready_queue = deque(tasks)
        
    completed = 0
    
    while completed < num_tasks:
        for i in range(num_cpus):
            if cpus[i] is not None:
                cpus[i].remaining -= 1
                cpu_quanta[i] -= 1
                
                if cpus[i].remaining <= 0:
                    completed += 1
                    cpus[i] = None
                elif cpu_quanta[i] <= 0:
                    if policy == "fixed":
                        queues[i].append(cpus[i])
                    else:
                        ready_queue.append(cpus[i])
                    cpus[i] = None
                    
        for i in range(num_cpus):
            if cpus[i] is None:
                t = None
                if policy == "fcfs":
                    if ready_queue:
                        t = ready_queue.popleft()
                elif policy == "fixed":
                    if queues[i]:
                        t = queues[i].popleft()
                elif policy == "last_processor":
                    if ready_queue:
                        best_idx = -1
                        for idx, task in enumerate(ready_queue):
                            if task.last_cpu == i:
                                best_idx = idx
                                break
                        if best_idx != -1:
                            t = ready_queue[best_idx]
                            del ready_queue[best_idx]
                        else:
                            t = ready_queue.popleft()
                elif policy == "minimum_intervening":
                    if ready_queue:
                        best_idx = 0
                        min_intervening = float('inf')
                        for idx, task in enumerate(ready_queue):
                            interv = 0
                            if task.last_cpu == i:
                                for hist_task in reversed(cpu_history[i]):
                                    if hist_task == task.task_id:
                                        break
                                    interv += 1
                            else:
                                interv = float('inf')
                                
                            if interv < min_intervening:
                                min_intervening = interv
                                best_idx = idx
                                
                        t = ready_queue[best_idx]
                        del ready_queue[best_idx]
                
                if t is not None:
                    cpus[i] = t
                    cpu_quanta[i] = quantum
                    cpu_history[i].append(t.task_id)
                    if t.last_cpu != -1 and t.last_cpu != i:
                        migrations += 1
                    t.last_cpu = i

        time += 1

    print(f"Policy: {policy}")
    print(f"  Total time: {time}")
    print(f"  Migrations: {migrations}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cpus', type=int, default=4)
    parser.add_argument('--tasks', type=int, default=20)
    parser.add_argument('--quantum', type=int, default=10)
    args = parser.parse_args()
    
    for policy in ['fcfs', 'fixed', 'last_processor', 'minimum_intervening']:
        run_simulation(policy, args.cpus, args.tasks, args.quantum)

if __name__ == "__main__":
    main()
