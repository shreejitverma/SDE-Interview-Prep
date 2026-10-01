#!/opt/aos-venv/bin/python
import sys
import os
import struct

MAGIC = b'LFS1'

class LFS:
    def __init__(self, log_file):
        self.log_file = log_file
        self.index = {} 
        self.recover()
        
    def recover(self):
        if not os.path.exists(self.log_file):
            return
        with open(self.log_file, 'rb') as f:
            while True:
                offset = f.tell()
                header = f.read(12)
                if not header or len(header) < 12:
                    break
                magic, obj_id, length = struct.unpack('<4sII', header)
                if magic != MAGIC:
                    print(f"Corrupt log at {offset}")
                    break
                self.index[obj_id] = (offset, length)
                f.seek(length, os.SEEK_CUR)
                
    def write(self, obj_id, data):
        data_bytes = data.encode('utf-8')
        length = len(data_bytes)
        with open(self.log_file, 'ab') as f:
            offset = f.tell()
            f.write(struct.pack('<4sII', MAGIC, obj_id, length))
            f.write(data_bytes)
            self.index[obj_id] = (offset, length)
        print(f"[LFS] Appended obj {obj_id} (len {length}) at offset {offset}")
            
    def read(self, obj_id):
        if obj_id not in self.index:
            return None
        offset, length = self.index[obj_id]
        with open(self.log_file, 'rb') as f:
            f.seek(offset + 12)
            data = f.read(length).decode('utf-8')
            print(f"[LFS] Read obj {obj_id}: {data}")
            return data
            
    def clean(self):
        initial_size = os.path.getsize(self.log_file)
        new_log_file = self.log_file + ".tmp"
        new_index = {}
        with open(new_log_file, 'wb') as out_f, open(self.log_file, 'rb') as in_f:
            for obj_id, (offset, length) in self.index.items():
                in_f.seek(offset)
                entry = in_f.read(12 + length)
                new_offset = out_f.tell()
                out_f.write(entry)
                new_index[obj_id] = (new_offset, length)
        os.rename(new_log_file, self.log_file)
        self.index = new_index
        final_size = os.path.getsize(self.log_file)
        print(f"[LFS] Cleaned log. Size reduced from {initial_size} to {final_size} bytes.")

    def dump_index(self):
        print(f"[LFS] Index state:")
        for obj_id, (offset, length) in self.index.items():
            print(f"  Obj {obj_id}: offset={offset}, len={length}")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: lfs.py <logfile> <write|read|clean|index> ...")
        sys.exit(1)
        
    lfs = LFS(sys.argv[1])
    cmd = sys.argv[2]
    if cmd == 'write':
        lfs.write(int(sys.argv[3]), sys.argv[4])
    elif cmd == 'read':
        lfs.read(int(sys.argv[3]))
    elif cmd == 'clean':
        lfs.clean()
    elif cmd == 'index':
        lfs.dump_index()
