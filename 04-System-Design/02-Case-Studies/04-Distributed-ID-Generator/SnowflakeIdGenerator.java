/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Snowflake ID Generator (Distributed Unique IDs)
 * Description: Generates unique, time-sortable 64-bit IDs without a central coordinator (unlike DB auto-increment).
 * Structure: [Sign Bit (1)] | [Timestamp (41)] | [Datacenter ID (5)] | [Worker ID (5)] | [Sequence (12)]
 */

public class SnowflakeIdGenerator {
    private final long machineId;
    
    // Bits allocations
    private final long SEQUENCE_BITS = 12;
    private final long MACHINE_ID_BITS = 10;
    
    // Max values
    private final long MAX_MACHINE_ID = ~(-1L << MACHINE_ID_BITS);
    private final long MAX_SEQUENCE = ~(-1L << SEQUENCE_BITS);
    
    // Shifts
    private final long MACHINE_ID_SHIFT = SEQUENCE_BITS;
    private final long TIMESTAMP_SHIFT = SEQUENCE_BITS + MACHINE_ID_BITS;
    
    // Epoch (e.g., 2023-01-01) - Custom start time to fit 41 bits
    private final long EPOCH = 1672531200000L; 

    private long lastTimestamp = -1L;
    private long sequence = 0L;

    public SnowflakeIdGenerator(long machineId) {
        if (machineId > MAX_MACHINE_ID || machineId < 0) {
            throw new IllegalArgumentException("Machine ID out of range");
        }
        this.machineId = machineId;
    }

    public synchronized long nextId() {
        long currentTimestamp = System.currentTimeMillis();

        if (currentTimestamp < lastTimestamp) {
            throw new RuntimeException("Clock moved backwards. Refusing to generate ID.");
        }

        if (currentTimestamp == lastTimestamp) {
            // Same millisecond: increment sequence
            sequence = (sequence + 1) & MAX_SEQUENCE;
            if (sequence == 0) {
                // Sequence exhausted, wait for next millisecond
                while (currentTimestamp <= lastTimestamp) {
                    currentTimestamp = System.currentTimeMillis();
                }
            }
        } else {
            // New millisecond: reset sequence
            sequence = 0L;
        }

        lastTimestamp = currentTimestamp;

        // Construct ID
        return ((currentTimestamp - EPOCH) << TIMESTAMP_SHIFT) |
               (machineId << MACHINE_ID_SHIFT) |
               sequence;
    }

    public static void main(String[] args) {
        SnowflakeIdGenerator gen = new SnowflakeIdGenerator(1); // Machine 1
        
        System.out.println("Generating IDs:");
        for (int i = 0; i < 5; i++) {
            System.out.println(gen.nextId());
        }
    }
}
