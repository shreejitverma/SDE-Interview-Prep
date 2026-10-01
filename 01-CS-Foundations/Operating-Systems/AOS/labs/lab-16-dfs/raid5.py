#!/opt/aos-venv/bin/python
import sys
import os
import time

def write_raid5(input_file, disk_prefix):
    with open(input_file, 'rb') as f:
        data = f.read()
    
    if len(data) % 4 != 0:
        data += b'\0' * (4 - (len(data) % 4))
    
    chunk_size = len(data) // 4
    
    d0 = bytearray(data[0:chunk_size])
    d1 = bytearray(data[chunk_size:2*chunk_size])
    d2 = bytearray(data[2*chunk_size:3*chunk_size])
    d3 = bytearray(data[3*chunk_size:4*chunk_size])
    
    parity = bytearray(chunk_size)
    
    start_t = time.monotonic()
    for i in range(chunk_size):
        parity[i] = d0[i] ^ d1[i] ^ d2[i] ^ d3[i]
    end_t = time.monotonic()
    
    for i, d in enumerate([d0, d1, d2, d3, parity]):
        with open(f"{disk_prefix}_{i}.dat", "wb") as f:
            f.write(d)
            
    print(f"[RAID] Wrote {len(data)} bytes across 4 data + 1 parity disks.")
    print(f"[RAID] Parity computation took {(end_t - start_t)*1000:.2f} ms")

def read_raid5(output_file, disk_prefix):
    disks = []
    for i in range(4):
        with open(f"{disk_prefix}_{i}.dat", "rb") as f:
            disks.append(f.read())
            
    with open(output_file, 'wb') as f:
        for chunk in disks:
            f.write(chunk)
    print(f"[RAID] Read from 4 disks to {output_file}.")

def reconstruct_raid5(missing_disk_id, disk_prefix):
    missing_disk_id = int(missing_disk_id)
    remaining = []
    for i in range(5):
        if i != missing_disk_id:
            with open(f"{disk_prefix}_{i}.dat", "rb") as f:
                remaining.append(bytearray(f.read()))
                
    chunk_size = len(remaining[0])
    
    start_t = time.monotonic()
    reconstructed = bytearray(chunk_size)
    for i in range(chunk_size):
        v = 0
        for disk_data in remaining:
            v ^= disk_data[i]
        reconstructed[i] = v
    end_t = time.monotonic()
    
    with open(f"{disk_prefix}_{missing_disk_id}.dat", "wb") as f:
        f.write(reconstructed)
        
    print(f"[RAID] Reconstructed disk {missing_disk_id} from remaining disks.")
    print(f"[RAID] Reconstruction took {(end_t - start_t)*1000:.2f} ms")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: raid5.py <write|read|reconstruct> <args...>")
        sys.exit(1)
        
    cmd = sys.argv[1]
    if cmd == 'write':
        write_raid5(sys.argv[2], sys.argv[3])
    elif cmd == 'read':
        read_raid5(sys.argv[2], sys.argv[3])
    elif cmd == 'reconstruct':
        reconstruct_raid5(sys.argv[2], sys.argv[3])
