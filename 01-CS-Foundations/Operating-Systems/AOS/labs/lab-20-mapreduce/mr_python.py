#!/opt/aos-venv/bin/python
import sys
import re
from collections import defaultdict

def map_func(line):
    words = re.findall(r'[a-zA-Z]+', line)
    return [(word.lower(), 1) for word in words]

def reduce_func(key, values):
    return (key, sum(values))

def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <file>")
        sys.exit(1)
        
    filename = sys.argv[1]
    
    # map phase
    mapped = []
    with open(filename, 'r') as f:
        for line in f:
            mapped.extend(map_func(line))
            
    # shuffle phase
    shuffled = defaultdict(list)
    for k, v in mapped:
        shuffled[k].append(v)
        
    # reduce phase
    reduced = []
    for k in sorted(shuffled.keys()):
        reduced.append(reduce_func(k, shuffled[k]))
        
    # output (sort by count descending)
    reduced.sort(key=lambda x: x[1], reverse=True)
    for k, v in reduced:
        print(f"{v:7d} {k}")

if __name__ == '__main__':
    main()
