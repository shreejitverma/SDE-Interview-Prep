#!/opt/aos-venv/bin/python
"""RAID-5 over five files: striped data with parity rotating across the disks.

Each stripe has 4 data blocks and 1 parity block; the parity block of stripe s lives on
disk (4 - s % 5), so parity load spreads over all disks (RAID-4 would pin it to one).
The original length is kept in an 8-byte header on every disk so reads drop the padding.

    raid5.py write <input> <prefix>        # writes <prefix>_0.dat .. <prefix>_4.dat
    raid5.py read <output> <prefix>        # reassembles the original bytes
    raid5.py reconstruct <disk> <prefix>   # rebuilds one lost disk from the other four
"""
import struct
import sys
import time

DISKS = 5
BLOCK = 4  # bytes per block, tiny so a short input still spans several stripes
HDR = struct.Struct(">Q")


def xor(blocks):
    out = bytearray(BLOCK)
    for b in blocks:
        for i in range(BLOCK):
            out[i] ^= b[i]
    return bytes(out)


def parity_disk(stripe):
    return (DISKS - 1) - stripe % DISKS


def write_raid5(input_file, prefix):
    data = open(input_file, "rb").read()
    per_stripe = BLOCK * (DISKS - 1)
    padded = data + b"\0" * (-len(data) % per_stripe)
    disks = [bytearray(HDR.pack(len(data))) for _ in range(DISKS)]
    t0 = time.monotonic()
    for s in range(len(padded) // per_stripe):
        chunk = padded[s * per_stripe:(s + 1) * per_stripe]
        blocks = [chunk[i * BLOCK:(i + 1) * BLOCK] for i in range(DISKS - 1)]
        p = parity_disk(s)
        data_disks = [d for d in range(DISKS) if d != p]
        for d, b in zip(data_disks, blocks):
            disks[d] += b
        disks[p] += xor(blocks)
    t1 = time.monotonic()
    for i, d in enumerate(disks):
        open(f"{prefix}_{i}.dat", "wb").write(d)
    print(f"[RAID] Wrote {len(data)} bytes as {len(padded) // per_stripe} stripes over {DISKS} disks, parity rotating.")
    print(f"[RAID] Parity computation took {(t1 - t0) * 1000:.2f} ms")


def load(prefix):
    raw = [open(f"{prefix}_{i}.dat", "rb").read() for i in range(DISKS)]
    length = HDR.unpack(raw[0][:HDR.size])[0]
    return length, [r[HDR.size:] for r in raw]


def read_raid5(output_file, prefix):
    length, disks = load(prefix)
    out = bytearray()
    for s in range(len(disks[0]) // BLOCK):
        p = parity_disk(s)
        for d in range(DISKS):
            if d != p:
                out += disks[d][s * BLOCK:(s + 1) * BLOCK]
    open(output_file, "wb").write(out[:length])
    print(f"[RAID] Read {length} bytes from {DISKS - 1} data blocks per stripe into {output_file}.")


def reconstruct_raid5(missing, prefix):
    missing = int(missing)
    raw = {i: open(f"{prefix}_{i}.dat", "rb").read() for i in range(DISKS) if i != missing}
    header = next(iter(raw.values()))[:HDR.size]
    bodies = {i: r[HDR.size:] for i, r in raw.items()}
    t0 = time.monotonic()
    rebuilt = bytearray(header)
    for s in range(len(next(iter(bodies.values()))) // BLOCK):
        rebuilt += xor([b[s * BLOCK:(s + 1) * BLOCK] for b in bodies.values()])
    t1 = time.monotonic()
    open(f"{prefix}_{missing}.dat", "wb").write(rebuilt)
    print(f"[RAID] Reconstructed disk {missing} from the other {DISKS - 1} disks.")
    print(f"[RAID] Reconstruction took {(t1 - t0) * 1000:.2f} ms")


if __name__ == "__main__":
    if len(sys.argv) != 4 or sys.argv[1] not in ("write", "read", "reconstruct"):
        sys.exit(__doc__)
    {"write": write_raid5, "read": read_raid5, "reconstruct": reconstruct_raid5}[sys.argv[1]](sys.argv[2], sys.argv[3])
