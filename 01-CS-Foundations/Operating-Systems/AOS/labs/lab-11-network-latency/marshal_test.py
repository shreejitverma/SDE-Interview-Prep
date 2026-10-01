import time
import json
import struct
import os
import capsule_pb2

# We use time.perf_counter which on Linux is clock_gettime(CLOCK_MONOTONIC)
def measure_time(func, *args, iterations=100000):
    start = time.perf_counter()
    for _ in range(iterations):
        func(*args)
    end = time.perf_counter()
    return (end - start) / iterations * 1e9  # nanoseconds

def test_json():
    data = {
        "type_hash": "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "previous_node": "node_A",
        "payload": "hello world"
    }
    
    def serialize():
        return json.dumps(data).encode('utf-8')
    
    encoded = serialize()
    def deserialize():
        return json.loads(encoded.decode('utf-8'))
        
    s_time = measure_time(serialize)
    d_time = measure_time(deserialize)
    print(f"JSON     - Serialize: {s_time:6.1f} ns | Deserialize: {d_time:6.1f} ns | Size: {len(encoded)} bytes")

def test_struct():
    # Struct format: 64s for hash, 16s for node, 16s for payload
    fmt = "64s 16s 16s"
    type_hash = b"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
    prev_node = b"node_A".ljust(16, b'\0')
    payload = b"hello world".ljust(16, b'\0')
    
    def serialize():
        return struct.pack(fmt, type_hash, prev_node, payload)
        
    encoded = serialize()
    def deserialize():
        return struct.unpack(fmt, encoded)
        
    s_time = measure_time(serialize)
    d_time = measure_time(deserialize)
    print(f"Struct   - Serialize: {s_time:6.1f} ns | Deserialize: {d_time:6.1f} ns | Size: {len(encoded)} bytes")

def test_protobuf():
    data = capsule_pb2.CapsuleMessage()
    data.type_hash = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
    data.previous_node = "node_A"
    data.payload = b"hello world"
    
    def serialize():
        return data.SerializeToString()
        
    encoded = serialize()
    def deserialize():
        msg = capsule_pb2.CapsuleMessage()
        msg.ParseFromString(encoded)
        return msg
        
    s_time = measure_time(serialize)
    d_time = measure_time(deserialize)
    print(f"Protobuf - Serialize: {s_time:6.1f} ns | Deserialize: {d_time:6.1f} ns | Size: {len(encoded)} bytes")

if __name__ == "__main__":
    print("Marshaling Cost Comparison (Medians implicitly represented over multiple iterations):")
    test_struct()
    test_json()
    test_protobuf()
