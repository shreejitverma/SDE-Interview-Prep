import bisect
import json
import os

class TemporalStream:
    def __init__(self, filename="stream.json", gc_window=300):
        self.filename = filename
        self.gc_window = gc_window
        self.timestamps = []
        self.items = {}
        self._load()

    def _load(self):
        if os.path.exists(self.filename):
            with open(self.filename, 'r') as f:
                data = json.load(f)
                for t_str, item in data.items():
                    t = float(t_str)
                    self.timestamps.append(t)
                    self.items[t] = item
            self.timestamps.sort()

    def _save(self):
        with open(self.filename, 'w') as f:
            json.dump({str(t): v for t, v in self.items.items()}, f)

    def put(self, item, timestamp):
        if timestamp not in self.items:
            bisect.insort(self.timestamps, timestamp)
        self.items[timestamp] = item
        self._save()

    def get(self, timestamp):
        if not self.timestamps:
            return None
        idx = bisect.bisect_right(self.timestamps, timestamp)
        if idx == 0:
            return None
        return self.items[self.timestamps[idx - 1]]

    def get_range(self, start_time, end_time):
        idx_start = bisect.bisect_left(self.timestamps, start_time)
        idx_end = bisect.bisect_right(self.timestamps, end_time)
        return [(self.timestamps[i], self.items[self.timestamps[i]]) for i in range(idx_start, idx_end)]

    def gc(self, current_time):
        cutoff = current_time - self.gc_window
        idx = bisect.bisect_left(self.timestamps, cutoff)
        for i in range(idx):
            t = self.timestamps[i]
            del self.items[t]
        self.timestamps = self.timestamps[idx:]
        self._save()

def run_demo():
    print("--- Temporal Stream Demo ---")
    store = TemporalStream("demo_stream.json", gc_window=50)
    print("Putting items at timestamps 10, 20, 30...")
    store.put("Event A", 10)
    store.put("Event B", 20)
    store.put("Event C", 30)
    print(f"Get at T=15: {store.get(15)}")
    print(f"Get at T=25: {store.get(25)}")
    print(f"Get range T=15 to T=35: {store.get_range(15, 35)}")
    print("Running GC at T=65 (window 50)...")
    store.gc(65)
    print(f"Get at T=15: {store.get(15)}")
    print(f"Get at T=25: {store.get(25)}")
    os.remove("demo_stream.json")
    print("Demo complete.\n")

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        store = TemporalStream("test_stream.json", gc_window=50)
        store.put("A", 10)
        store.put("B", 20)
        store.put("C", 30)
        assert store.get(15) == "A"
        assert store.get(20) == "B"
        assert store.get(25) == "B"
        assert store.get(5) is None
        assert store.get_range(15, 25) == [(20, "B")]
        store.gc(65)
        assert store.get(15) is None
        assert store.get(25) == "B"
        store2 = TemporalStream("test_stream.json", gc_window=50)
        assert store2.get(25) == "B"
        assert store2.get(35) == "C"
        os.remove("test_stream.json")
        print("PASS: TemporalStream tests")
    else:
        run_demo()
