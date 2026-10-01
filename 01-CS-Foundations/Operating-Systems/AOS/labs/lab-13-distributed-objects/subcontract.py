import time

class Subcontract:
    def invoke(self, method, args):
        raise NotImplementedError

class SingletonSubcontract(Subcontract):
    def __init__(self, target_obj):
        self.target_obj = target_obj

    def invoke(self, method, args):
        times = []
        result = None
        func = getattr(self.target_obj, method)
        for _ in range(10):
            start = time.monotonic_ns()
            result = func(*args)
            end = time.monotonic_ns()
            times.append(end - start)
        times.sort()
        median = times[5]
        print(f"[Singleton] invocation median time (10 runs): {median} ns")
        return result

class ReplicatedSubcontract(Subcontract):
    def __init__(self, target_objs):
        self.target_objs = target_objs

    def invoke(self, method, args):
        times = []
        final_result = None
        for _ in range(10):
            start = time.monotonic_ns()
            results = []
            for obj in self.target_objs:
                func = getattr(obj, method)
                results.append(func(*args))
            end = time.monotonic_ns()
            times.append(end - start)
            final_result = results[0]
        times.sort()
        median = times[5]
        print(f"[Replicated] invoked {len(self.target_objs)} replicas, invocation median time (10 runs): {median} ns")
        return final_result

class Stub:
    def __init__(self, subcontract):
        self._subcontract = subcontract

    def do_work(self, data):
        return self._subcontract.invoke("do_work", [data])

class Worker:
    def __init__(self, name):
        self.name = name

    def do_work(self, data):
        # Simulate some work
        time.sleep(0.01)
        return f"Worker {self.name} processed: {data}"

if __name__ == "__main__":
    w1 = Worker("A")
    w2 = Worker("B")
    w3 = Worker("C")

    # Singleton subcontract
    sc1 = SingletonSubcontract(w1)
    stub1 = Stub(sc1)
    print("Singleton output:", stub1.do_work("test data"))

    # Replicated subcontract
    sc2 = ReplicatedSubcontract([w1, w2, w3])
    stub2 = Stub(sc2)
    print("Replicated output:", stub2.do_work("test data"))
