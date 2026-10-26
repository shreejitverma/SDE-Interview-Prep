#!/usr/bin/env python3
"""Pure standard-library parser for Git packfiles and index v2 files.

Reads .idx v2 and .pack files, decodes variable-length headers, delta chains
(OBJ_OFS_DELTA and OBJ_REF_DELTA), verifies object integrity (CRC32 and SHA-1),
and inspects reconstructed objects against git cat-file.
"""

from __future__ import annotations

import collections
import hashlib
import os
import struct
import subprocess
import sys
import zlib


TYPE_NAMES = {1: "commit", 2: "tree", 3: "blob", 4: "tag"}


def parse_packfile(repo_dir: str) -> None:
    pack_dir = os.path.join(repo_dir, ".git", "objects", "pack")
    idx_files = sorted(f for f in os.listdir(pack_dir) if f.endswith(".idx"))
    if not idx_files:
        print("No .idx files found in pack directory", file=sys.stderr)
        sys.exit(1)

    idx_basename = idx_files[0]
    pack_basename = idx_basename[:-4] + ".pack"
    idx_path = os.path.join(pack_dir, idx_basename)
    pack_path = os.path.join(pack_dir, pack_basename)

    with open(idx_path, "rb") as f:
        idx_data = f.read()
    with open(pack_path, "rb") as f:
        pack_data = f.read()

    # 1. Parse .idx v2 header
    magic, version = struct.unpack(">4sI", idx_data[:8])
    if magic != b"\xfftOc" or version != 2:
        raise ValueError(f"Unsupported pack index: magic={magic!r}, version={version}")

    # 2. Parse 256-entry first-level fanout table
    fanout = struct.unpack(">256I", idx_data[8 : 8 + 1024])
    total_objects = fanout[255]

    # 3. Parse table of sorted 20-byte SHA-1 object names
    pos = 8 + 1024
    oids = [idx_data[pos + i * 20 : pos + (i + 1) * 20].hex() for i in range(total_objects)]
    pos += total_objects * 20

    # 4. Parse table of 4-byte CRC32 values
    crcs = struct.unpack(f">{total_objects}I", idx_data[pos : pos + total_objects * 4])
    pos += total_objects * 4

    # 5. Parse table of 4-byte packfile offsets
    offsets_raw = struct.unpack(f">{total_objects}I", idx_data[pos : pos + total_objects * 4])
    pos += total_objects * 4

    # 6. Parse table of 8-byte large offsets (for offsets >= 2 GiB)
    large_offsets = idx_data[pos : len(idx_data) - 40]

    offsets: list[int] = []
    for off in offsets_raw:
        if off & 0x80000000:
            large_idx = off & 0x7FFFFFFF
            real_off = struct.unpack(">Q", large_offsets[large_idx * 8 : (large_idx + 1) * 8])[0]
            offsets.append(real_off)
        else:
            offsets.append(off)

    # Trailers: the .pack ends with SHA-1(all preceding pack bytes); the .idx ends with a copy of
    # that pack checksum followed by SHA-1(all preceding idx bytes).
    pack_sum_ok = hashlib.sha1(pack_data[:-20]).digest() == pack_data[-20:]
    idx_sum_ok = hashlib.sha1(idx_data[:-20]).digest() == idx_data[-20:]
    link_ok = idx_data[-40:-20] == pack_data[-20:]
    if not (pack_sum_ok and idx_sum_ok and link_ok):
        raise ValueError(f"trailer checksum failure: pack={pack_sum_ok} idx={idx_sum_ok} link={link_ok}")
    if any(a > b for a, b in zip(fanout, fanout[1:])):
        raise ValueError("fanout table is not monotonic")
    raw_names = idx_data[8 + 1024 : 8 + 1024 + total_objects * 20]

    def find_offset(oid_hex: str) -> int:
        # Fanout narrows the search to names sharing the first byte, then binary search.
        name = bytes.fromhex(oid_hex)
        lo = fanout[name[0] - 1] if name[0] else 0
        hi = fanout[name[0]]
        while lo < hi:
            mid = (lo + hi) // 2
            cand = raw_names[mid * 20 : mid * 20 + 20]
            if cand < name:
                lo = mid + 1
            elif cand > name:
                hi = mid
            else:
                return offsets[mid]
        raise KeyError(oid_hex)

    # 7. Parse .pack header: 'PACK', version 2, object count
    pack_magic, pack_ver, pack_obj_count = struct.unpack(">4sII", pack_data[:12])
    if pack_magic != b"PACK" or pack_ver != 2 or pack_obj_count != total_objects:
        raise ValueError("Invalid pack header or object count mismatch between pack and idx")

    def parse_raw_entry(off: int) -> tuple[int, int, tuple[str, int | str] | None, bytes, int]:
        curr = off
        c = pack_data[curr]
        curr += 1
        type_id = (c >> 4) & 7
        size = c & 15
        shift = 4
        while c & 0x80:
            c = pack_data[curr]
            curr += 1
            size |= (c & 0x7F) << shift
            shift += 7

        base_info: tuple[str, int | str] | None = None
        if type_id == 6:  # OBJ_OFS_DELTA
            c = pack_data[curr]
            curr += 1
            ofs_val = c & 0x7F
            while c & 0x80:
                c = pack_data[curr]
                curr += 1
                ofs_val = ((ofs_val + 1) << 7) | (c & 0x7F)
            base_info = ("ofs", off - ofs_val)
        elif type_id == 7:  # OBJ_REF_DELTA
            base_info = ("ref", pack_data[curr : curr + 20].hex())
            curr += 20

        decompressor = zlib.decompressobj()
        decomp_data = decompressor.decompress(pack_data[curr:])
        comp_len = len(pack_data[curr:]) - len(decompressor.unused_data)
        end = curr + comp_len
        crc = zlib.crc32(pack_data[off:end]) & 0xFFFFFFFF
        return type_id, size, base_info, decomp_data, crc

    def apply_delta(base_data: bytes, delta: bytes) -> bytes:
        p = 0

        def read_varint() -> int:
            nonlocal p
            res = 0
            s = 0
            while True:
                b = delta[p]
                p += 1
                res |= (b & 0x7F) << s
                s += 7
                if not (b & 0x80):
                    break
            return res

        base_sz = read_varint()
        target_sz = read_varint()
        if len(base_data) != base_sz:
            raise ValueError(f"Base size mismatch: expected {base_sz}, got {len(base_data)}")

        out = bytearray()
        while p < len(delta):
            cmd = delta[p]
            p += 1
            if cmd & 0x80:  # Copy instruction
                cp_off = 0
                cp_sz = 0
                if cmd & 0x01:
                    cp_off |= delta[p]
                    p += 1
                if cmd & 0x02:
                    cp_off |= delta[p] << 8
                    p += 1
                if cmd & 0x04:
                    cp_off |= delta[p] << 16
                    p += 1
                if cmd & 0x08:
                    cp_off |= delta[p] << 24
                    p += 1
                if cmd & 0x10:
                    cp_sz |= delta[p]
                    p += 1
                if cmd & 0x20:
                    cp_sz |= delta[p] << 8
                    p += 1
                if cmd & 0x40:
                    cp_sz |= delta[p] << 16
                    p += 1
                if cp_sz == 0:
                    cp_sz = 0x10000
                out.extend(base_data[cp_off : cp_off + cp_sz])
            elif cmd > 0:  # Insert instruction
                out.extend(delta[p : p + cmd])
                p += cmd
            else:
                raise ValueError("Reserved instruction opcode 0x00")

        if len(out) != target_sz:
            raise ValueError(f"Target size mismatch: expected {target_sz}, got {len(out)}")
        return bytes(out)

    resolved: dict[int, tuple[str, bytes, int]] = {}

    def resolve(off: int) -> tuple[str, bytes, int]:
        # Walk the delta chain down to a full object, then apply deltas back up (no recursion,
        # so a deep chain cannot exhaust the Python stack).
        chain: list[tuple[int, bytes]] = []
        cur = off
        while cur not in resolved:
            type_id, _, base_info, decomp_data, _ = parse_raw_entry(cur)
            if type_id in TYPE_NAMES:
                resolved[cur] = (TYPE_NAMES[type_id], decomp_data, 0)
                break
            if type_id not in (6, 7) or base_info is None:
                raise ValueError(f"Unknown type id {type_id} at offset {cur}")
            chain.append((cur, decomp_data))
            cur = int(base_info[1]) if type_id == 6 else find_offset(str(base_info[1]))
        base_type, base_data, base_depth = resolved[cur]
        for delta_off, delta in reversed(chain):
            base_data = apply_delta(base_data, delta)
            base_depth += 1
            resolved[delta_off] = (base_type, base_data, base_depth)
        return resolved[off]

    type_counts: dict[str, int] = collections.Counter()
    depth_histogram: dict[int, int] = collections.Counter()
    object_info: dict[str, tuple[str, int, int]] = {}

    for oid, off, exp_crc in zip(oids, offsets, crcs):
        _, _, _, _, crc = parse_raw_entry(off)
        if crc != exp_crc:
            raise ValueError(f"CRC32 mismatch for {oid}: index={exp_crc:08x}, computed={crc:08x}")
        tname, data, depth = resolve(off)
        raw_header = f"{tname} {len(data)}\0".encode("ascii")
        computed_oid = hashlib.sha1(raw_header + data).hexdigest()
        if computed_oid != oid:
            raise ValueError(f"SHA-1 mismatch for {oid}: reconstructed={computed_oid}")

        type_counts[tname] += 1
        depth_histogram[depth] += 1
        object_info[oid] = (tname, len(data), depth)

    print(f"Objects total: {total_objects}")
    print()
    print("Object counts by type:")
    for tname in sorted(type_counts.keys()):
        print(f"  {tname}: {type_counts[tname]}")
    print()
    print("Delta-chain depth histogram:")
    for depth in sorted(depth_histogram.keys()):
        print(f"  chain length {depth}: {depth_histogram[depth]}")
    print()
    print("Trailer checksums: pack SHA-1 OK, idx SHA-1 OK, idx copy of pack checksum OK")
    print()
    # Compare every reconstructed object byte-for-byte with git's own reader.
    batch_in = "".join(f"{oid}\n" for oid in oids).encode()
    batch = subprocess.run(["git", "cat-file", "--batch"], cwd=repo_dir, input=batch_in,
                           capture_output=True, check=True).stdout
    matched, pos = 0, 0
    for oid in oids:
        nl = batch.index(b"\n", pos)
        _, gtype, gsize = batch[pos:nl].decode().split()
        body = batch[nl + 1 : nl + 1 + int(gsize)]
        pos = nl + 1 + int(gsize) + 1
        tname, data, _ = resolve(find_offset(oid))
        matched += tname == gtype and data == body
    print(f"Byte-for-byte match with git cat-file --batch: {matched}/{total_objects} objects")
    print()
    print("Spot checks (fanout binary search, then git cat-file -s):")

    test_targets = [
        ("ee4882e3d2e51878fc2e905dbcf7e762dd5c5c18", "base blob, depth 0"),
        ("552d35aa8847740dfa7b1417590d32b7c478bdd6", "delta blob, depth 1"),
        ("97b8f0240c0a7de782f9d49f50d9f276766cb3d8", "delta blob, depth 6"),
    ]

    for target_oid, label in test_targets:
        if target_oid not in object_info:
            print(f"  {target_oid} ({label}): NOT FOUND MISMATCH")
            continue
        _, rec_size, _ = object_info[target_oid]
        find_offset(target_oid)  # must resolve through the fanout table
        proc = subprocess.run(
            ["git", "cat-file", "-s", target_oid],
            cwd=repo_dir,
            capture_output=True,
            text=True,
            check=True,
        )
        git_size = int(proc.stdout.strip())
        status = "OK" if rec_size == git_size else "MISMATCH"
        print(f"  {target_oid} ({label}): size={rec_size} (git: {git_size}) {status}")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: packfile.py <repo-path>", file=sys.stderr)
        sys.exit(1)
    parse_packfile(sys.argv[1])
