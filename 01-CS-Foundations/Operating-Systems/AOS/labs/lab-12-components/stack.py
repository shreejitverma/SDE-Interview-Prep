#!/opt/aos-venv/bin/python
import sys
import time
import hashlib
import json

class Packet:
    def __init__(self, data, seq_num=0, checksum=None, is_fragment=False, frag_id=0, frag_offset=0, is_last_frag=True):
        self.data = data
        self.seq_num = seq_num
        self.checksum = checksum
        self.is_fragment = is_fragment
        self.frag_id = frag_id
        self.frag_offset = frag_offset
        self.is_last_frag = is_last_frag

class NetworkLayer:
    def __init__(self):
        self.peer = None

    def set_peer(self, peer):
        self.peer = peer

    def send(self, packet):
        if self.peer:
            self.peer.receive(packet)

    def receive(self, packet):
        pass

class BottomLayer(NetworkLayer):
    def __init__(self, upper):
        super().__init__()
        self.upper = upper
        self.bypass_hook = None

    def receive(self, packet):
        if self.bypass_hook and self.bypass_hook(packet):
            return
        self.upper.receive(packet)

class ChecksumLayer:
    def __init__(self, upper, lower):
        self.upper = upper
        self.lower = lower
        self.lower.upper = self

    def _compute_checksum(self, data):
        return hashlib.md5(data.encode('utf-8')).hexdigest()[:8]

    def send(self, packet):
        packet.checksum = self._compute_checksum(packet.data)
        self.lower.send(packet)

    def receive(self, packet):
        expected = self._compute_checksum(packet.data)
        if packet.checksum == expected:
            self.upper.receive(packet)

class OrderingLayer:
    def __init__(self, upper, lower):
        self.upper = upper
        self.lower = lower
        self.lower.upper = self
        self.send_seq = 0
        self.recv_seq = 0
        self.buffer = {}

    def send(self, packet):
        packet.seq_num = self.send_seq
        self.send_seq += 1
        self.lower.send(packet)

    def receive(self, packet):
        if packet.seq_num == self.recv_seq:
            self.upper.receive(packet)
            self.recv_seq += 1
            while self.recv_seq in self.buffer:
                next_pkt = self.buffer.pop(self.recv_seq)
                self.upper.receive(next_pkt)
                self.recv_seq += 1
        elif packet.seq_num > self.recv_seq:
            self.buffer[packet.seq_num] = packet

class FragmentationLayer:
    def __init__(self, upper, lower, mtu=64):
        self.upper = upper
        self.lower = lower
        self.lower.upper = self
        self.mtu = mtu
        self.frag_id_counter = 0
        self.reassembly_buffer = {}

    def send(self, packet):
        if len(packet.data) <= self.mtu:
            self.lower.send(packet)
        else:
            data = packet.data
            frag_id = self.frag_id_counter
            self.frag_id_counter += 1
            offset = 0
            while offset < len(data):
                chunk = data[offset:offset+self.mtu]
                is_last = (offset + self.mtu) >= len(data)
                frag_pkt = Packet(
                    data=chunk,
                    is_fragment=True,
                    frag_id=frag_id,
                    frag_offset=offset,
                    is_last_frag=is_last
                )
                self.lower.send(frag_pkt)
                offset += self.mtu

    def receive(self, packet):
        if not packet.is_fragment:
            self.upper.receive(packet)
        else:
            fid = packet.frag_id
            if fid not in self.reassembly_buffer:
                self.reassembly_buffer[fid] = []
            self.reassembly_buffer[fid].append(packet)
            
            if packet.is_last_frag:
                frags = self.reassembly_buffer[fid]
                frags.sort(key=lambda p: p.frag_offset)
                full_data = "".join(p.data for p in frags)
                assembled = Packet(data=full_data)
                del self.reassembly_buffer[fid]
                self.upper.receive(assembled)


class DummyLayer:
    def __init__(self, upper, lower):
        self.upper = upper
        self.lower = lower
        self.lower.upper = self

    def send(self, packet):
        self.lower.send(packet)

    def receive(self, packet):
        self.upper.receive(packet)

class AppLayer:
    def __init__(self, lower):
        self.lower = lower
        if self.lower:
            self.lower.upper = self
        self.received_messages = []

    def send(self, data):
        packet = Packet(data=data)
        self.lower.send(packet)

    def receive(self, packet):
        self.received_messages.append(packet.data)

class FastPathBypass:
    def __init__(self, app, ordering, checksum, bottom):
        self.app = app
        self.ordering = ordering
        self.checksum = checksum
        self.bottom = bottom

    def _compute_checksum(self, data):
        return hashlib.md5(data.encode('utf-8')).hexdigest()[:8]

    def send(self, data, use_bypass=True):
        if use_bypass and len(data) <= 64:
            pkt = Packet(data=data)
            pkt.seq_num = self.ordering.send_seq
            self.ordering.send_seq += 1
            pkt.checksum = self._compute_checksum(data)
            self.bottom.send(pkt)
        else:
            self.app.send(data)

class ProtocolStack:
    def __init__(self):
        self.bottom = BottomLayer(None)
        self.checksum = ChecksumLayer(None, self.bottom)
        self.d1 = DummyLayer(None, self.checksum)
        self.d2 = DummyLayer(None, self.d1)
        self.d3 = DummyLayer(None, self.d2)
        self.d4 = DummyLayer(None, self.d3)
        self.d5 = DummyLayer(None, self.d4)
        self.d6 = DummyLayer(None, self.d5)
        self.d7 = DummyLayer(None, self.d6)
        self.d8 = DummyLayer(None, self.d7)
        self.d9 = DummyLayer(None, self.d8)
        self.d10 = DummyLayer(None, self.d9)
        self.d11 = DummyLayer(None, self.d10)
        self.d12 = DummyLayer(None, self.d11)
        self.d13 = DummyLayer(None, self.d12)
        self.d14 = DummyLayer(None, self.d13)
        self.d15 = DummyLayer(None, self.d14)
        self.d16 = DummyLayer(None, self.d15)
        self.d17 = DummyLayer(None, self.d16)
        self.d18 = DummyLayer(None, self.d17)
        self.d19 = DummyLayer(None, self.d18)
        self.d20 = DummyLayer(None, self.d19)
        self.ordering = OrderingLayer(None, self.d20)
        self.frag = FragmentationLayer(None, self.ordering)
        self.app = AppLayer(self.frag)
        self.bypass = FastPathBypass(self.app, self.ordering, self.checksum, self.bottom)

class NetworkEmulator:
    def __init__(self, enable_bypass=False):
        self.stack1 = ProtocolStack()
        self.stack2 = ProtocolStack()
        
        self.stack1.bottom.set_peer(self.stack2.bottom)
        self.stack2.bottom.set_peer(self.stack1.bottom)

        if enable_bypass:
            def bypass_hook_2(packet):
                if (not packet.is_fragment and 
                    packet.seq_num == self.stack2.ordering.recv_seq and
                    not self.stack2.ordering.buffer):
                    expected_csum = self.stack2.bypass._compute_checksum(packet.data)
                    if packet.checksum == expected_csum:
                        self.stack2.ordering.recv_seq += 1
                        self.stack2.app.received_messages.append(packet.data)
                        return True
                return False

            self.stack2.bottom.bypass_hook = bypass_hook_2

def benchmark(messages, use_bypass):
    net = NetworkEmulator(enable_bypass=use_bypass)
    start = time.perf_counter()
    for msg in messages:
        if use_bypass:
            net.stack1.bypass.send(msg, use_bypass=True)
        else:
            net.stack1.app.send(msg)
    end = time.perf_counter()
    
    # Optional: check delivery to ensure correctness during benchmark
    # assert len(net.stack2.app.received_messages) == len(messages)
    
    return (end - start) * 1000000 / len(messages)

def test():
    net = NetworkEmulator(enable_bypass=False)
    net.stack1.app.send("Hello slow path")
    assert "Hello slow path" in net.stack2.app.received_messages

    net = NetworkEmulator(enable_bypass=True)
    net.stack1.bypass.send("Hello fast path", use_bypass=True)
    assert "Hello fast path" in net.stack2.app.received_messages

    long_msg = "A" * 100
    net.stack1.bypass.send(long_msg, use_bypass=True)
    assert long_msg in net.stack2.app.received_messages

    # Out of order bypass test
    net = NetworkEmulator(enable_bypass=True)
    # create artificial out of order
    pkt1 = Packet(data="Msg1", seq_num=0, checksum=net.stack1.bypass._compute_checksum("Msg1"))
    pkt2 = Packet(data="Msg2", seq_num=1, checksum=net.stack1.bypass._compute_checksum("Msg2"))
    
    # send out of order
    net.stack2.bottom.receive(pkt2)
    net.stack2.bottom.receive(pkt1)
    
    assert net.stack2.app.received_messages == ["Msg1", "Msg2"]
    print("PASS: Protocol stack tests")

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        test()
        sys.exit(0)

    print("--- Ensemble Micro-Protocol Bypass Benchmark ---")
    messages = ["Ensemble packet data"] * 50000
    
    slow_time = benchmark(messages, use_bypass=False)
    print(f"Unoptimized layered stack: {slow_time:.2f} us / msg")
    
    fast_time = benchmark(messages, use_bypass=True)
    print(f"Optimized CCP fast-path:   {fast_time:.2f} us / msg")
    
    savings = slow_time - fast_time
    pct = (savings / slow_time) * 100
    print(f"Savings:                   {savings:.2f} us ({pct:.1f}%)")

if __name__ == "__main__":
    main()
