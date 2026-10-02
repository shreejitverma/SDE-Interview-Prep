#!/opt/aos-venv/bin/python
import sys
import random

class Task:
    def __init__(self, task_id, task_type):
        self.task_id = task_id
        self.task_type = task_type
        self.state = 'idle'
        self.worker = None

def simulate(m_tasks, r_tasks, num_workers, straggler_prob, enable_backup):
    free_workers = list(range(num_workers))
    
    map_tasks = [Task(i, 'map') for i in range(m_tasks)]
    reduce_tasks = [Task(i, 'reduce') for i in range(r_tasks)]
    
    clock = 0
    in_progress = [] # list of dicts: {'task_id': id, 'type': type, 'worker': w, 'finish_time': t, 'is_backup': bool}
    
    def schedule(tasks, task_type):
        nonlocal clock
        completed = set()
        
        while len(completed) < len(tasks):
            # Assign idle tasks
            for task in tasks:
                if task.state == 'idle' and free_workers:
                    worker = free_workers.pop(0)
                    task.state = 'in-progress'
                    task.worker = worker
                    duration = 10 if random.random() < straggler_prob else 1
                    if duration == 10:
                        print(f"[{clock:2d}] {task_type}-{task.task_id} scheduled on worker {worker} (STRAGGLER)")
                    else:
                        print(f"[{clock:2d}] {task_type}-{task.task_id} scheduled on worker {worker}")
                    in_progress.append({
                        'task_id': task.task_id,
                        'type': task_type,
                        'worker': worker,
                        'finish_time': clock + duration,
                        'is_backup': False
                    })
            
            # Backup tasks
            if enable_backup:
                # If all tasks are either completed or in-progress, we can schedule backups
                idle_tasks = [t for t in tasks if t.state == 'idle']
                if not idle_tasks and free_workers:
                    # Find tasks that are in-progress but don't have a backup yet
                    # In a real system, we might only backup those running longer than average
                    in_prog_ids = [p['task_id'] for p in in_progress if p['type'] == task_type]
                    # Get unique ids
                    unique_ids = set(in_prog_ids)
                    for tid in unique_ids:
                        instances = [p for p in in_progress if p['task_id'] == tid and p['type'] == task_type]
                        if len(instances) == 1 and free_workers:
                            worker = free_workers.pop(0)
                            print(f"[{clock:2d}] {task_type}-{tid} BACKUP scheduled on worker {worker}")
                            duration = 1 # Backup is unlikely to straggle for simulation purposes
                            in_progress.append({
                                'task_id': tid,
                                'type': task_type,
                                'worker': worker,
                                'finish_time': clock + duration,
                                'is_backup': True
                            })

            clock += 1
            
            # Process completions
            finished = [p for p in in_progress if p['finish_time'] == clock]
            in_progress[:] = [p for p in in_progress if p['finish_time'] > clock]
            
            for p in finished:
                tid = p['task_id']
                if tid not in completed:
                    completed.add(tid)
                    status = "BACKUP completed" if p['is_backup'] else "completed"
                    print(f"[{clock:2d}] {task_type}-{tid} {status} on worker {p['worker']}")
                    # Free the worker
                    free_workers.append(p['worker'])
                    # If this was the backup, or if the original finished, we should also free 
                    # the other worker if it's still running. MapReduce can kill it, or just let it finish.
                    # For simulation, we'll remove it from in_progress to free the worker.
                    other_instances = [o for o in in_progress if o['task_id'] == tid and o['type'] == task_type]
                    for o in other_instances:
                        print(f"[{clock:2d}] {task_type}-{tid} killing other instance on worker {o['worker']}")
                        free_workers.append(o['worker'])
                    in_progress[:] = [o for o in in_progress if not (o['task_id'] == tid and o['type'] == task_type)]
                else:
                    # Already completed, just free worker
                    free_workers.append(p['worker'])

    print(f"--- Starting Map Phase (M={m_tasks}) ---")
    schedule(map_tasks, 'map')
    print(f"--- Starting Reduce Phase (R={r_tasks}) ---")
    schedule(reduce_tasks, 'reduce')
    
    print(f"Job finished at time {clock}")

if __name__ == '__main__':
    print("Simulation 1: NO backup tasks")
    random.seed(42)
    simulate(m_tasks=10, r_tasks=5, num_workers=4, straggler_prob=0.2, enable_backup=False)
    
    print("\nSimulation 2: WITH backup tasks")
    random.seed(42)
    simulate(m_tasks=10, r_tasks=5, num_workers=4, straggler_prob=0.2, enable_backup=True)
