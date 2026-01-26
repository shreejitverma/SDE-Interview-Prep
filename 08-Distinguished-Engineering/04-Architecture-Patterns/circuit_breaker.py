# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Circuit Breaker Pattern (Resiliency)
# Description: Prevents cascading failures. If a service fails repeatedly, stop calling it ("Open" state).

import time
import random

class CircuitBreaker:
    def __init__(self, failure_threshold=3, recovery_timeout=5):
        self.failure_threshold = failure_threshold
        self.recovery_timeout = recovery_timeout
        self.failures = 0
        self.state = "CLOSED" # CLOSED (Normal), OPEN (Failing), HALF-OPEN (Testing)
        self.last_failure_time = 0

    def call(self, func, *args):
        if self.state == "OPEN":
            if time.time() - self.last_failure_time > self.recovery_timeout:
                print("Circuit HALF-OPEN: Trying one request...")
                self.state = "HALF-OPEN"
            else:
                print("Circuit OPEN: Fast fail.")
                return None

        try:
            result = func(*args)
            self._success()
            return result
        except Exception as e:
            self._failure()
            print(f"Call failed: {e}")
            return None

    def _success(self):
        self.failures = 0
        self.state = "CLOSED"
        print("Call Successful. Circuit CLOSED.")

    def _failure(self):
        self.failures += 1
        self.last_failure_time = time.time()
        if self.failures >= self.failure_threshold:
            self.state = "OPEN"
            print("Failure Threshold Reached. Circuit OPEN.")

# Simulation
def unreliable_service():
    if random.random() < 0.7:
        raise Exception("Timeout")
    return "200 OK"

if __name__ == "__main__":
    cb = CircuitBreaker(failure_threshold=2, recovery_timeout=2)

    for i in range(10):
        print(f"\nRequest {i+1}:")
        cb.call(unreliable_service)
        time.sleep(0.5)
