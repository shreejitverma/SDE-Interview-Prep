class HardwareClock:
    def __init__(self, frequency=1.0):
        self.ticks = 0
        self.frequency = frequency

    def tick(self, dt):
        self.ticks += dt * self.frequency

    def get_ticks(self):
        return self.ticks

class FeedbackClock:
    def __init__(self, hw_clock):
        self.hw_clock = hw_clock
        self.frequency_adjust = 1.0

    def get_time(self):
        return self.hw_clock.get_ticks() * self.frequency_adjust

    def sync(self, true_time):
        current_time = self.get_time()
        error = true_time - current_time
        # Proportional feedback control
        self.frequency_adjust += error * 0.05

class FeedforwardClock:
    def __init__(self, hw_clock):
        self.hw_clock = hw_clock
        self.offset = 0.0
        self.multiplier = 1.0
        self.last_sync_hw = 0
        self.last_sync_true = 0

    def get_time(self):
        return self.offset + (self.hw_clock.get_ticks() - self.last_sync_hw) * self.multiplier

    def sync(self, true_time):
        hw_ticks = self.hw_clock.get_ticks()
        if hw_ticks > self.last_sync_hw:
            # Estimate new multiplier based on interval
            self.multiplier = (true_time - self.last_sync_true) / (hw_ticks - self.last_sync_hw)
        self.offset = true_time
        self.last_sync_hw = hw_ticks
        self.last_sync_true = true_time

def run_simulation():
    print("--- Clock Sync Simulation ---")
    hw = HardwareClock(frequency=0.90) # 10% slow
    fb_clock = FeedbackClock(hw)
    ff_clock = FeedforwardClock(hw)
    
    true_time = 0.0
    dt = 1.0
    
    print(f"{'Step':>4} | {'True':>8} | {'Hardware':>8} | {'Feedback':>8} | {'Feedforward':>11}")
    print("-" * 52)
    for step in range(1, 16):
        true_time += dt
        hw.tick(dt)
        
        fb_time = fb_clock.get_time()
        ff_time = ff_clock.get_time()
        print(f"{step:4d} | {true_time:8.2f} | {hw.get_ticks():8.2f} | {fb_time:8.2f} | {ff_time:11.2f}")
        
        # Sync every 5 steps
        if step % 5 == 0:
            print("       *** SYNC ***")
            fb_clock.sync(true_time)
            ff_clock.sync(true_time)

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        hw = HardwareClock(frequency=0.5)
        hw.tick(10)
        assert hw.get_ticks() == 5.0
        ff = FeedforwardClock(hw)
        ff.sync(10.0)
        hw.tick(10)
        ff.sync(20.0)
        assert ff.multiplier == 2.0
        print("PASS: ClockSync tests")
    else:
        run_simulation()
