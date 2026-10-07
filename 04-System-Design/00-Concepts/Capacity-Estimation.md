---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Designing Data-Intensive Applications, Martin Kleppmann"
  - "The Datacenter as a Computer, Barroso, Clidaras, and Holzle"
---

# Capacity Estimation

## TL;DR

Estimate to find which resource runs out first.
The usual candidates are write QPS, the hot key, storage after replication and retention, and cross-zone bandwidth.
A diagram that does not change when the peak factor changes is not a design.

## The four numbers

Use one day as about `10^5` seconds.
86,400 is close enough that powers of ten will not lie to you.

```python
SECONDS_PER_DAY = 86_400

def average_qps(dau: float, actions_per_user_per_day: float) -> float:
    return dau * actions_per_user_per_day / SECONDS_PER_DAY

def storage_bytes(writes_per_day: float, record_bytes: int, days: int, replication: int) -> float:
    return writes_per_day * record_bytes * days * replication
```

Peak QPS is average QPS times a peak factor.
Daily products often see 2 to 5.
A launch, a sports final, or a flash sale sees 10 to 100, and the peak is a spike rather than a tall hour.
State the factor out loud.

Storage is writes per day, times bytes per record, times retention days, times replication.
Bandwidth is QPS times bytes on the wire.
Cache memory is the working set, not the full history.
A 1 percent working set at a 99 percent hit rate is a different machine from a cache of everything.

## Worked feed

100 million daily active users each open the feed 20 times.
Average read QPS is `1e8 * 20 / 86400`, about 23,000.
At a peak factor of 4, the read path is about 90,000 QPS.
Each user posts once a day.
Write QPS is about 1,200 average and about 4,600 at the same peak.
A 1 KB post kept for 365 days with 3 replicas is `1e8 * 1000 * 365 * 3`, about 110 TB.
The read path is a cache and fan-out problem.
The write path is a storage problem.
They do not size the same boxes.
[[05-Social-Media-Feed/design|The feed study]] uses this split.

## What to say next

After the number, name the bottleneck.
A single Kafka partition, a single Redis shard, a primary key lock, or a cross-region RTT will saturate before the cluster CPU does.
[[Backpressure-and-Tail-Latency]] is how you behave when the estimate was wrong.
[[Replication-and-Quorums]] multiplies storage and multiplies write latency.
Leave both effects in the estimate.

## Pitfalls

- Counting average QPS as the capacity target.
- Forgetting replication and indexes.
  An index can be as large as the table.
- Treating a peak of 10 seconds like a peak of 4 hours.
  Autoscaling will not arrive for the 10-second case.
- Estimating payload without the fan-out.
  A chat message stored once and pushed to 500 devices is 500 sends.

## Interview questions

1. Walk storage for a 1 KB event at 10,000 writes per second, kept 30 days, with 3 replicas.
2. Which number changes if the read-to-write ratio goes from 10:1 to 1000:1.
3. Why is a peak factor of 2 a dangerous assumption for ticket sales.
4. What do you add to the estimate when the cache hit rate drops from 95 percent to 50 percent.

## Further reading

- [[01-URL-Shortener/design|The URL shortener]] for a small, fully worked version of this method.
- [[SLOs-and-Observability]] so the estimate becomes a dashboard rather than a slide.
