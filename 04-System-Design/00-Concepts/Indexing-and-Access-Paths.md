---
type: concept
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "The Log-Structured Merge-Tree, O'Neil et al., Acta Informatica 1996"
  - "Designing Data-Intensive Applications, Martin Kleppmann, chapter 3"
---

# Indexing and Access Paths

## TL;DR

An index is a second data structure that answers one class of question without reading the whole table.
The class you choose decides your write amplification, your read amplification, and whether a query can stay on one shard.

## The structures

| Structure | Good question | Pain |
|---|---|---|
| B+ tree | Point lookup and a short range, with updates | Random writes split pages |
| LSM tree | Write-heavy point lookup, scans of recent data | Reads check several levels; compaction steals IO |
| Hash index | Exact key, in memory | No range, and a resize is expensive |
| Inverted index | "Documents containing these terms" | Phrase and freshness make postings heavy |
| Secondary index on a shard | A query that is not the shard key | Either scatter to every shard, or a global index that is its own distributed table |

A B+ tree keeps keys in sorted leaves, with a high fanout so the height stays near three or four for billions of rows.
The leaf is the unit of caching.
[[MySQL-and-InnoDB]] clusters the row in the primary B+ tree.
[[PostgreSQL-Architecture]] keeps the heap separate and indexes point at tuple ids.
Those are different costs for an update that touches one indexed column.

An LSM tree absorbs writes in memory and flushes sorted runs.
Reads merge runs.
Compaction bounds the number of runs and rewrites data several times over its life.
That rewrite is the write amplification.
[[LSM-Tree]] is the lab.
[[Apache-Cassandra]] and many time-series stores are the production shape.
Use an LSM when writes are the product.
Use a B+ tree when you update rows in place and read short ranges.

## Sharded secondary indexes

If users are sharded by `user_id`, a query for `email = X` is not local.
A local secondary index means every shard has an email index, and the query fans out.
A global secondary index shards by email, so the query is one hop, and every user write becomes a distributed transaction or an async projection.
Name which one you picked.
[[Partitioning-and-Sharding]] is the placement half of this choice.
[[Elasticsearch-and-Apache-Solr]] is a global inverted index, fed asynchronously, and it is allowed to lag.

## Worked fanout

A B+ tree page of 16 KB and a 32-byte key-plus-pointer holds about 500 children.
Three levels hold `500^3`, 125 million leaves, before you count how many rows sit in a leaf.
The point of the arithmetic is the height, not the slogan "logs are fast".
The disk still pays one random IO per level on a cold cache.

## Pitfalls

- Indexing every column writes every column twice and still does not answer a leading-wildcard search.
- A covering index that matches the query can skip the heap.
  A covering index you never read is pure write cost.
- Building a new index by locking the table is an outage.
  Online index builds exist, and they have a catch-up phase you must include in [[Schema-Evolution-and-Migration]].
- A vector index (HNSW and related graphs) answers approximate nearest neighbor.
  It is not a B+ tree, and the recall is part of the SLO.

## Interview questions

1. Why does an LSM slow down reads as compaction falls behind.
2. Users are sharded by id, and you must look up by email. Draw both index plans and the write each one adds.
3. When does a hash index beat a B+ tree.
4. What does BM25 change in an inverted index, relative to storing only term lists.

## Further reading

- O'Neil et al., The Log-Structured Merge-Tree, 1996.
- [[PostgreSQL-Architecture]] and [[MySQL-and-InnoDB]] for the two relational layouts.
- [[12-Typeahead-Search/design|Typeahead]] for a prefix structure that is not the same as a search inverted index.
