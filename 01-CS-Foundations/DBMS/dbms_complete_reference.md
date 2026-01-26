# Complete Database Management Systems Reference - Comprehensive Guide

## Summary

This document contains an exhaustive list of **199 essential Database Management Systems keywords and concepts** with extensive explanations organized into 11 major categories.

---

## CORE DBMS CONCEPTS (23 keywords)

### Fundamental Concepts

**Database Management System (DBMS)** - Software suite that allows users to define, store, query, and manage data efficiently. Acts as intermediary between applications and physical data storage. Handles concurrent access, maintains data integrity, ensures security, and optimizes performance. Examples: MySQL, PostgreSQL, MongoDB, Oracle, SQL Server.

**Database** - Organized collection of related data stored persistently and managed by a DBMS. Structured to represent real-world entities and relationships. Can be centralized (single location) or distributed (multiple locations).

**Data Model** - Formal representation of data structure, relationships, and constraints. Different models suit different purposes: relational (tables), document (JSON), key-value (simple maps), graph (nodes/edges), object-oriented (objects with methods).

**Schema** - Blueprint/structure defining database organization. Specifies table definitions, column names, data types, constraints, indexes, and relationships. Static for most relational databases, flexible for document databases.

**Relation** - Two-dimensional table organized in rows and columns - fundamental unit of relational databases. Has specific structure with defined attributes and constraints.

**Tuple** - Single row/record in a relation representing one instance of the entity. Example: (CustomerID:1, Name:'John', Email:'john@example.com').

**Attribute** - Column in a relation representing a data property or characteristic. Example: CustomerID, CustomerName, CustomerEmail are attributes of Customers relation.

**Entity** - Thing or concept in the real world represented in database. Examples: Customer, Order, Product, Employee. Becomes table in relational model.

**Relationship** - Association between entities representing how they connect. Example: Customer places Order (one-to-many), Order contains Product (many-to-many).

**Primary Key** - Unique identifier for each row in a table ensuring no duplicates. Every relation must have primary key. Can be single attribute (simple) or multiple attributes (composite).

**Foreign Key** - Column(s) referencing primary key in another table establishing relationships. Enforces referential integrity - prevents orphaned records. Values must exist in referenced table.

**Index** - Data structure accelerating data retrieval by organizing records for quick lookup. Trades storage space for speed. Examples: B-tree (range queries), hash (equality queries).

**Query** - Request for data retrieval, modification, or management. SQL is primary language for relational databases. Examples: SELECT, INSERT, UPDATE, DELETE operations.

**Transaction** - Logical unit of work consisting of one or more database operations. All-or-nothing semantics: either all succeed (commit) or all fail (rollback). Ensures consistency.

**ACID Properties** - Guarantees provided by transaction processing: Atomicity (all-or-nothing), Consistency (valid state to valid state), Isolation (concurrent independence), Durability (persistence after commit). Foundation of reliable database operations.

### Relational Model Basics

**Relational Model** - Mathematical framework organizing data in tables with rows and columns. Developed by Edgar F. Codd, foundation of modern SQL databases. Provides simplicity, flexibility, and powerful query capabilities.

**Normalization** - Process of organizing data to minimize redundancy and dependencies while preserving data integrity. Results in better database design, easier maintenance, but sometimes requires more joins for queries.

**First Normal Form (1NF)** - First normalization step: eliminate repeating groups, ensure all values are atomic (indivisible). Each column contains single value, not arrays or lists.

**Second Normal Form (2NF)** - Build from 1NF: remove partial dependencies where non-key attributes depend on subset of composite key. All non-key attributes must depend on entire primary key.

**Third Normal Form (3NF)** - Remove transitive dependencies where non-key attributes depend on other non-key attributes. Most common practical form, balances redundancy with performance.

**Boyce-Codd Normal Form (BCNF)** - Strictest normal form where every determinant is a candidate key. Rarely necessary but ensures strongest data integrity properties.

**Denormalization** - Intentionally introducing redundancy to improve query performance. Violates normalization principles but reduces expensive joins. Trade-off between consistency and speed.

**Cardinality** - Describes relationship multiplicity between entities: one-to-one (1:1), one-to-many (1:N), many-to-many (M:N). Fundamental to relationship design and schema structure.

---

## DATA MANAGEMENT & QUERIES (24 keywords)

### SQL Fundamentals

**SQL (Structured Query Language)** - Standardized language for querying and manipulating relational databases. ANSI/ISO standardized, universally supported. Most common database interaction method.

**DDL (Data Definition Language)** - SQL commands defining/modifying database structure. Commands: CREATE (table/database), ALTER (modify structure), DROP (delete). Defines schema.

**DML (Data Manipulation Language)** - SQL commands for manipulating data within tables. Commands: SELECT (retrieve), INSERT (add), UPDATE (modify), DELETE (remove). Operates on data.

**DCL (Data Control Language)** - SQL commands controlling user access and permissions. Commands: GRANT (assign privileges), REVOKE (remove privileges). Enforces security.

**TCL (Transaction Control Language)** - SQL commands managing transaction execution. Commands: COMMIT (confirm changes), ROLLBACK (undo changes), SAVEPOINT (transaction checkpoint). Ensures data consistency.

**SELECT Statement** - Primary query retrieving data from database. Syntax: SELECT columns FROM table WHERE condition. Most commonly used SQL command.

**JOIN** - Combining rows from multiple tables based on common column(s). Different types serve different purposes: INNER, LEFT, RIGHT, FULL, CROSS joins.

**INNER JOIN** - Returns only rows where condition matches in both tables. Intersection of datasets, excludes non-matching rows. Most restrictive join type.

**LEFT JOIN** - Returns all rows from left table plus matching rows from right. Non-matching right rows show NULL. Preserves left table completeness.

**GROUP BY** - Aggregating rows into groups for summary calculations. Creates groups based on specified column(s), aggregate functions (SUM, COUNT, AVG) applied per group.

**HAVING** - Filtering groups after aggregation (unlike WHERE which filters rows). Applied after GROUP BY. Example: HAVING COUNT(*) > 5 selects groups with 5+ items.

**ORDER BY** - Sorting query results in ascending (ASC) or descending (DESC) order. Single or multiple columns. Affects presentation, not data structure.

**Subquery** - Query nested inside another query. Can appear in WHERE, FROM, or SELECT clauses. Enables complex queries by breaking into logical pieces.

**Aggregate Functions** - Functions operating on multiple rows returning single result: COUNT (rows), SUM (total), AVG (average), MIN (minimum), MAX (maximum).

**UNION** - Combining results from multiple SELECT statements. UNION removes duplicates, UNION ALL keeps them. Both queries must return compatible columns.

### Data Modification

**INSERT** - Adding new rows to a table. Provides column values or uses INSERT SELECT from another query. Triggers constraints validation.

**UPDATE** - Modifying existing row values. WHERE clause specifies which rows to update. Missing WHERE updates all rows (dangerous).

**DELETE** - Removing rows from table. WHERE clause specifies which rows to delete. Missing WHERE deletes all rows.

**TRUNCATE** - Fast removal of all rows from table. Resets identity counter, cannot use WHERE clause. Faster than DELETE but less flexible.

**Constraint** - Rule enforcing data integrity and business rules. Types: PRIMARY KEY (unique ID), FOREIGN KEY (referential), UNIQUE (uniqueness), CHECK (condition), NOT NULL (presence).

**UNIQUE Constraint** - Ensures all values in column(s) are unique. Differs from PRIMARY KEY (multiple possible, can be NULL). Prevents duplicate entries.

**CHECK Constraint** - Validates column values meet specific condition. Example: CHECK (age >= 18) ensures age validity. Applied on insert/update.

**NOT NULL Constraint** - Ensures column always contains value, never NULL. Enforces data presence. Fundamental integrity constraint.

**DEFAULT** - Provides default value if no value supplied during insert. Example: DEFAULT GETDATE() provides current timestamp automatically.

---

## DATABASE ARCHITECTURE (21 keywords)

### Core Components

**Query Processor** - Component parsing, validating, and optimizing SQL queries. Transforms SQL text into executable plan. Heart of DBMS query processing.

**Query Optimizer** - Determines most efficient way to execute query among multiple possibilities. Cost-based optimizer calculates execution cost for different plans. Critical performance component.

**Execution Engine** - Component executing optimized query plan. Applies operations in optimal order determined by optimizer. Interfaces with storage engine for data access.

**Storage Engine** - Component managing physical data storage, retrieval, and organization. Interface between logical query operations and disk I/O. Different engines have different properties (InnoDB vs MyISAM).

**Buffer Manager** - Caches frequently accessed data in memory reducing disk I/O. Manages buffer pool, decides which pages stay in memory. Critical for performance as RAM 100-1000x faster than disk.

**Transaction Manager** - Manages transaction execution ensuring ACID properties. Coordinates atomicity through log-based recovery, isolation through concurrency control, durability through persistence.

**Recovery Manager** - Restores database to consistent state after failures (crashes, power loss). Uses write-ahead logs replaying committed transactions, undoing uncommitted ones.

**Concurrency Control Manager** - Manages simultaneous access by multiple users/processes preventing conflicts. Implements locking or MVCC. Enables multiple readers/writers without data corruption.

**Security Manager** - Controls user access and enforces security policies. Authentication (who you are), authorization (what you can do). Checks privileges for every operation.

**Catalog/Data Dictionary** - Metadata repository describing database structure and constraints. Stores table definitions, column information, indexes, security definitions. Central source of schema truth.

### Storage Structures

**File Organization** - How database records physically organized on disk. Choices: heap (unordered), sorted (ordered by key), hashed (distributed by hash). Affects access performance.

**Heap File** - Unordered record storage, insertion at end. Simple organization but requires full table scan for most queries. Used when no specific order needed.

**Sorted File** - Records ordered by key value enabling binary search. Efficient for range queries and sequential access. Insertion expensive (must maintain sort order).

**Hashed File** - Records distributed across buckets using hash function. Direct access to specific records. Poor for range queries (hash function non-sequential).

**Block** - Basic storage unit on disk (typically 4KB, configurable). Minimum read/write unit. Multiple records fit in one block.

**Record** - Logical data unit combining related fields. Can be fixed-length (simple, wastes space) or variable-length (complex, space-efficient).

**Page** - In-memory copy of disk block cached in buffer. Buffer manager manages page movement between disk and memory. Page cache unifies memory management.

**Extent** - Contiguous blocks allocated as unit for table storage. Reduces fragmentation, improves sequential access. Multiple extents comprise table storage.

**B-Tree Index** - Balanced tree structure for efficient range queries. Maintains sorted order, self-balancing on insertion/deletion. Standard index type for relational databases.

**Hash Index** - Hash table structure for point lookups. O(1) average case lookup but terrible for range queries. Used for equality conditions.

**Bitmap Index** - Bit array index for low-cardinality columns. Efficient for filtering on columns with few distinct values. Common in data warehouses.

---

## TRANSACTION MANAGEMENT (21 keywords)

### ACID Properties

**Atomicity** - All-or-nothing property: transaction either fully committed or fully rolled back. No partial updates despite failures. Ensures consistency from transaction perspective.

**Consistency** - Database valid state to valid state transition. All constraints satisfied before and after transaction. Prevents invalid states from persisting.

**Isolation** - Concurrent transactions don't interfere with each other. Appear to execute sequentially. Prevents dirty reads, non-repeatable reads, phantom reads.

**Durability** - Committed data survives failures (crashes, power loss). Persists on durable storage (disk). Once committed, data guaranteed permanent.

**Dirty Read** - Reading uncommitted data from concurrent transaction. Problematic if that transaction rolls back. Violates isolation guarantee.

**Non-Repeatable Read** - Reading same data twice yields different results within same transaction. Another transaction modified data between reads. Problematic for consistency.

**Phantom Read** - Range query returns different rows on subsequent execution within same transaction. Another transaction inserted/deleted matching rows. Problematic for concurrent modifications.

**Lost Update** - Two transactions modify same data, one update lost. Concurrent write conflict. Prevented by isolation and concurrency control.

### Isolation Levels

**Read Uncommitted** - Lowest isolation level allowing dirty reads. Transactions see uncommitted changes from others. Fastest but unsafe for critical data.

**Read Committed** - Standard isolation level preventing dirty reads. Other transactions' committed changes visible. Allows non-repeatable reads and phantoms. Balance of safety and performance.

**Repeatable Read** - Prevents dirty and non-repeatable reads but allows phantom reads. Snapshot consistency, reads consistent at transaction start. MySQL default isolation.

**Serializable** - Highest isolation level, fully sequential execution appearance. All anomalies prevented. Slowest but safest. Rarely needed in practice.

**Snapshot Isolation** - Transaction sees consistent snapshot of database at start time. Uses MVCC for consistency. Prevents most anomalies, excellent concurrency.

### Concurrency Control

**Pessimistic Locking** - Acquires locks before reading/modifying preventing conflicts proactively. Conservative approach, prevents all conflicts. May cause reduced concurrency.

**Optimistic Locking** - No locks acquired initially, conflicts checked at commit. Rollback if conflicts detected. Better concurrency if conflicts rare.

**Read Lock (Shared Lock)** - Multiple transactions can simultaneously read, prevents writes. Shared access, prevents modification conflicts. Released after read completes.

**Write Lock (Exclusive Lock)** - Only one transaction can write, blocks all reads and writes. Exclusive access, ensures no interference. Released after modification completes.

**Deadlock** - Circular wait situation where transactions wait for each other's locks indefinitely. Prevents progress, requires detection and recovery. Common with pessimistic locking.

**Lock Timeout** - Transaction waiting for lock exceeds timeout threshold and aborts. Prevents indefinite waits. Trade-off: too short causes false aborts, too long delays recovery.

**Two-Phase Locking (2PL)** - Guarantees serializability through lock phases: growing (acquire locks) then shrinking (release locks). No locks acquired after first release.

**MVCC (Multi-Version Concurrency Control)** - Maintains multiple data versions for concurrent access. Readers see version at transaction start, writers create new versions. Excellent concurrency without blocking.

---

## INDEXING & PERFORMANCE (19 keywords)

### Index Types

**Primary Index** - Index on primary key column(s). Unique, typically clustered on most systems. Main lookup path for record access.

**Secondary Index** - Index on non-key column enabling alternative access path. Multiple secondary indexes per table possible. Complements primary index for query optimization.

**Clustered Index** - Determines physical order of records on disk. One per table maximum. Data records sorted by clustered key.

**Non-Clustered Index** - Separate structure from data referencing records via pointer. Multiple per table, independent of record order. Doesn't affect record organization.

**Composite Index** - Index on multiple columns enabling efficient multi-column queries. Column order matters for query optimization. Prefix matches also work.

**Unique Index** - Enforces uniqueness constraint on indexed column(s). Ensures all indexed values distinct. Enables fast lookup and duplicate prevention.

**Full-Text Index** - Specialized index for text searching and pattern matching. MATCH/AGAINST operations in SQL. Common in content/document systems.

**Spatial Index** - Indexes geometric/geographic data (coordinates, boundaries). R-tree structures for spatial relationships. Used in mapping, GIS systems.

**Partial Index** - Index on subset of rows meeting condition. Reduced size for frequently queried subset. Improves space efficiency for common patterns.

### Query Optimization

**Query Plan** - Sequence of operations for executing query. Determined by optimizer, can have multiple valid plans. Shown by EXPLAIN command.

**Cost Model** - Estimates query execution cost in resources: CPU, I/O, memory. Optimization chooses lowest cost plan. Fundamental to query optimizer.

**Selectivity** - Fraction of total rows matching a condition. Higher selectivity (fewer rows) favors index use. Lower selectivity may favor table scan.

**Cardinality** - Number of distinct values in column. Affects index effectiveness. High cardinality columns good index candidates.

**Join Order** - Sequence of table joining dramatically affects performance. Smaller result sets first reduces total data volume. Optimizer determines optimal order.

**Index Scan** - Reading entire index sequentially. Used when few rows qualify or no applicable index available. Less efficient than index seek.

**Index Seek** - Directly accessing relevant index entries. More efficient than scan when condition selective. Jumps to specific entries.

**Table Scan** - Reading entire table sequentially. Full scan required when no applicable indexes. Slow on large tables but necessary sometimes.

**EXPLAIN/ANALYZE** - Shows query plan and optionally actual execution statistics. Identifies performance bottlenecks. Essential tool for query optimization.

**Execution Plan Visualization** - Graphical representation of query execution showing join operations and costs. Helps understand complex queries. Visual formats easier to interpret than text.

---

## DATABASE TYPES (17 keywords)

### Traditional Databases

**Relational Database (RDBMS)** - Data organized in tables with predefined schema and relationships. SQL-based, ACID compliant. Examples: MySQL, PostgreSQL, Oracle, SQL Server.

**SQL Database** - Database using SQL for querying. Inherently relational (all RDBMs are SQL but concept broader). Standard for traditional business applications.

**OLTP (Online Transaction Processing)** - Optimized for rapid, frequent transactions with small data sets. Many short read/write operations. Examples: e-commerce, banking, real-time systems.

**OLAP (Online Analytical Processing)** - Optimized for complex queries on large historical data sets. Few slow write operations, many complex reads. Examples: data warehouses, BI systems.

### NoSQL Databases

**NoSQL Database** - Non-relational database with flexible schema and horizontal scaling. Trade some consistency for availability and partition tolerance. Examples: MongoDB, Cassandra, Redis.

**Document Database** - Stores semi-structured documents (JSON/BSON) without predefined schema. Flexible nested data, schema evolution easy. Example: MongoDB, CouchDB.

**Key-Value Store** - Simplest data model: unique key maps to value. No schema, ultra-fast lookups. Example: Redis, Memcached. Good for caching, sessions.

**Wide-Column Store** - Data organized in column families (not tables). Columns grouped together for efficient access. Example: Cassandra, HBase. Good for time-series, analytics.

**Graph Database** - Stores data as nodes and edges representing entities and relationships. Excels at relationship queries. Example: Neo4j. Good for social networks, recommendations.

**Search Engine** - Specialized for full-text search and complex filtering. Inverted indexes for text. Example: Elasticsearch, Solr. Used for search, logging, analytics.

**Time-Series Database** - Optimized for timestamped data points (metrics, logs, events). High write throughput, efficient compression. Example: InfluxDB, Prometheus. Used for monitoring, metrics.

### Specialized Databases

**NewSQL Database** - Combines SQL functionality with NoSQL horizontal scalability. ACID transactions across distributed nodes. Example: CockroachDB, Google Spanner.

**Columnar Database** - Data stored column-wise instead of row-wise. Much faster analytical queries accessing subset of columns. Example: Redshift, ClickHouse.

**In-Memory Database** - Data primarily in RAM instead of disk. Ultra-fast access, persistence optional. Example: Redis, Memcached. Great for caching, real-time.

**Distributed Database** - Data distributed across multiple machines/locations. Horizontal scaling, high availability, geographic distribution. Example: Cassandra, DynamoDB.

**Data Warehouse** - Centralized repository for historical data analysis. Optimized for OLAP, complex queries. Example: Redshift, BigQuery, Snowflake.

**Data Lake** - Centralized repository storing raw data in native format. Schema-on-read flexibility, massive scale. Example: HDFS, S3, Delta Lake.

---

## DATA INTEGRITY (12 keywords)

### Constraints & Validation

**Data Integrity** - Accuracy, completeness, and consistency of data in database. Maintained through constraints, validation, and business logic.

**Entity Integrity** - Every entity has unique identifier, primary key never NULL. Fundamental integrity constraint.

**Referential Integrity** - Foreign keys reference valid primary keys, no orphaned records. Parent record must exist for child to reference it.

**Domain Integrity** - Attribute values within valid range/type. Data type specification, CHECK constraints, domain enforcement.

**User-Defined Integrity** - Custom business rules specific to application domain. Triggers, stored procedures enforce business logic.

**Constraint** - Rule preventing invalid data entry. Types: PRIMARY KEY, FOREIGN KEY, UNIQUE, CHECK, NOT NULL.

**Trigger** - Automated action on specific database event (INSERT, UPDATE, DELETE). Can be before (validation) or after (cascading actions).

**Stored Procedure** - Pre-compiled SQL code executed by database. Encapsulates business logic, ensures consistent execution, improves performance.

### Data Quality

**Data Validation** - Checking data meets format/range/rules before insertion. Catches errors early, prevents bad data.

**Data Cleansing** - Identifying and correcting invalid/inconsistent/duplicate data. Improves data quality, enables accurate analysis.

**Master Data Management (MDM)** - Centralized management of core business data ensuring single source of truth. Critical for consistency across systems.

**Data Governance** - Policies/procedures ensuring data quality, privacy, compliance. Access control, retention, auditing.

---

## ADVANCED DBMS FEATURES (13 keywords)

### Partitioning & Distribution

**Partitioning** - Dividing table into separate parts for manageability and performance. Improves query performance, simplifies maintenance.

**Range Partitioning** - Partitions based on column value ranges. Example: 2023 data separate from 2024. Good temporal separation.

**Hash Partitioning** - Partitions using hash function for even distribution. Distributes load evenly, no logical grouping.

**List Partitioning** - Partitions based on predefined value lists. Example: geographic regions (North/South/East/West).

**Sharding** - Distributing data across multiple database servers for horizontal scaling. Each shard stores subset, queries fan-out across shards.

**Replication** - Copying data across multiple servers for redundancy and availability. Creates copies for failover and read scaling.

**Master-Slave Replication** - Single master accepts writes, slaves replicate asynchronously. Read scaling, eventual consistency.

**Multi-Master Replication** - Multiple masters accept writes, replicate changes to each other. Higher availability, conflict resolution complexity.

### Performance Features

**Query Caching** - Storing query results in memory for reuse. Subsequent identical queries return cached results without execution.

**Connection Pooling** - Reusing database connections instead of creating new. Reduces connection overhead, improves throughput.

**Prepared Statements** - Pre-compiled queries with parameter placeholders. Faster execution, prevents SQL injection attacks.

**Batch Processing** - Executing multiple operations together. Reduces network roundtrips, improves throughput.

**Materialized View** - Pre-computed query results stored physically. Faster access than computing on demand, requires maintenance.

---

## SECURITY & BACKUP (17 keywords)

### Access Control

**Authentication** - Verifying user identity through credentials (username/password). First step of security, answers "who are you?"

**Authorization** - Controlling what authenticated user can access/perform. Second step of security, answers "what can you do?"

**Role-Based Access Control (RBAC)** - Users assigned roles with specific permissions. DBA role, Analyst role, Developer role. Simplifies permission management.

**User Account** - Unique login credential with associated privileges and quotas. Fundamental security unit.

**Privilege** - Permission to perform specific operation (SELECT, INSERT, UPDATE, DELETE) on specific objects.

**Grant** - Giving user specific privileges. GRANT SELECT ON table TO user.

**Revoke** - Removing previously granted privileges. REVOKE INSERT ON table FROM user.

### Data Protection

**Encryption** - Encoding data to prevent unauthorized access. At-rest (storage) and in-transit (network) encryption.

**Encryption At-Rest** - Data encoded on storage media preventing access if storage compromised. Disk encryption, encrypted backups.

**Encryption In-Transit** - Data encoded during network transmission preventing interception. SSL/TLS for connections, encrypted tunnels.

**Hashing** - One-way transformation ensuring data integrity. Passwords hashed (SHA-256, bcrypt) for secure storage.

**Backup** - Copy of database for disaster recovery. Full, incremental, differential backup strategies.

**Full Backup** - Complete database copy. Time-consuming, large, but complete recovery point.

**Incremental Backup** - Changes since last backup only. Fast, space-efficient, requires all previous backups to restore.

**Differential Backup** - Changes since last full backup. Faster than full, fewer backups needed for restore.

**Recovery** - Restoring database from backup after failure. Point-in-time recovery using logs.

**Point-in-Time Recovery** - Restoring database to specific moment using transaction logs. Essential for disaster recovery.

---

## ADMINISTRATION (14 keywords)

### Database Administration

**Database Administrator (DBA)** - Professional managing database systems. Responsibilities: backups, security, performance tuning, maintenance, updates.

**Monitoring** - Tracking database performance: queries, resource usage, errors. Proactive issue identification.

**Tuning** - Optimizing database for performance. Index optimization, query rewriting, configuration adjustment, resource allocation.

**Maintenance** - Regular tasks ensuring database health: backups, updates, index maintenance, consistency checks, log rotation.

**Indexing Strategy** - Designing index structure based on query patterns and usage. Balances query performance against update overhead.

**Table Statistics** - Information about data distribution (cardinality, distribution) used by query optimizer. Must keep current for accurate query plans.

**Query Profiling** - Analyzing query execution identifying bottlenecks. Slow query logs, profiling tools reveal performance issues.

**Capacity Planning** - Estimating future storage and compute resource needs. Prevents resource exhaustion.

**Upgrade Management** - Planning, testing, and executing database software upgrades. Manages downtime, ensures compatibility, tests before production.

### Database Maintenance

**Vacuum** - Cleaning up dead rows from DELETE operations and reclaiming space. PostgreSQL VACUUM, maintenance task.

**Analyze** - Updating table statistics used by query optimizer. PostgreSQL ANALYZE maintains accurate statistics.

**Defragmentation** - Reorganizing fragmented data reducing wasted space. Index REBUILD/REORGANIZE, space optimization.

**Consistency Check** - Verifying database integrity, detecting corruption. DBCC CHECKDB (SQL Server), CHECK TABLE (MySQL).

**Log Rotation** - Archiving transaction logs freeing space. Prevents transaction logs filling disk, enables point-in-time recovery.

---

## ADVANCED TOPICS (18 keywords)

### Distributed Systems

**Distributed Database** - Database spread across multiple locations/computers. Horizontal scaling, high availability, geographic distribution. Complex but scalable.

**CAP Theorem** - Fundamental trade-off: Consistency (all nodes same), Availability (always operational), Partition tolerance (network failures handled). Choose 2 of 3.

**Consistency Model** - Guarantees on data visibility consistency across system. Strong (immediate), eventual (eventual convergence), causal (causality order).

**Eventual Consistency** - Temporary inconsistency but converges to consistent state eventually. Used for high availability systems. Acceptable for most applications.

**Strong Consistency** - All nodes see same data immediately. Available in centralized systems, difficult in distributed systems.

**Consensus Algorithm** - Protocol for distributed nodes agreeing on state. Raft, Paxos ensure agreement despite failures.

**Quorum** - Majority (>50%) of nodes must agree before operation. Ensures fault tolerance in distributed systems.

### Query Processing

**Query Parser** - Validates SQL syntax and structure. First step, catches grammatical errors.

**Query Translator** - Converts SQL to internal representation. Creates abstract syntax tree for further processing.

**Query Optimizer** - Determines most efficient execution plan. Cost-based optimizer evaluates multiple plans.

**Query Execution** - Actually running optimized plan retrieving results. Final step, executes selected plan.

**Explain Plan** - Shows how database will execute query. EXPLAIN output reveals optimizer decisions.

**Execution Statistics** - Runtime information about query execution. Rows examined, I/O operations, execution time.

### Advanced Indexing

**Covering Index** - Index containing all columns needed for query. Query resolved entirely from index without table access. Maximum efficiency.

**Index Selectivity** - Percentage of rows returned by index condition. Higher selectivity favors index use, lower selectivity may favor table scan.

**Index Cardinality** - Number of unique values in indexed column. Affects index effectiveness for filtering.

**Redundant Index** - Index providing no benefit over other indexes. Should be removed for space efficiency.

**Index Maintenance** - Keeping indexes optimized as data changes. Rebuild (reorganize physically), reorganize (defragment), update statistics.

---

## COMPLETE STATISTICS

**Total Database Management Systems Keywords & Concepts: 199**

### Breakdown by Category:

| Category | Keywords | Focus Area |
|----------|----------|-----------|
| Core DBMS Concepts | 23 | Fundamentals and data model |
| Data Management & Queries | 24 | SQL and query operations |
| Database Architecture | 21 | Internal components and storage |
| Transaction Management | 21 | ACID, isolation, concurrency |
| Indexing & Performance | 19 | Index types and optimization |
| Database Types | 17 | SQL, NoSQL, specialized systems |
| Data Integrity | 12 | Constraints and data quality |
| Advanced DBMS Features | 13 | Partitioning, distribution |
| Security & Backup | 17 | Access control and recovery |
| Administration | 14 | DBA tasks and maintenance |
| Advanced Topics | 18 | Distributed systems, query processing |

---

## KEY CONCEPTS FOR FINANCIAL SYSTEMS & TRADING

Given your background in **quantitative finance and trading systems**:

### Critical for Trading Platforms:

**OLTP Focus** - Trading systems are OLTP: many small, fast transactions (order entry, fills, updates) not batch processing.

**Latency Optimization** - Index design crucial for sub-millisecond order lookups. Partitioning by trader/symbol reduces search space.

**Data Integrity** - ACID transactions essential: order state changes must be atomic (submitted → filled → settled).

**Concurrency Control** - Multiple traders placing orders simultaneously requires robust isolation. Deadlocks unacceptable in trading.

**Real-Time Analytics** - Some trading systems blend OLTP (order entry) with OLAP (P&L reporting) requiring hybrid architectures.

**Distributed Systems** - Multi-region trading systems use distributed databases (Cassandra, CockroachDB) for failover and geographic redundancy.

**Replication** - Master-slave replication for real-time reporting. Read replicas for analytics without impacting production.

**Partitioning Strategies** - Hash partitioning by order ID for load balancing, range partitioning by date for historical data.

### Database Selection for Trading:

**PostgreSQL** - Excellent for trading platforms: ACID compliance, rich data types, JSON support for market data, proven reliability.

**MySQL/InnoDB** - Widespread, good performance, proven for high-volume transactional systems.

**NoSQL Considerations** - Key-value stores (Redis) for caching orders/quotes, time-series databases (InfluxDB) for market data and metrics.

**Data Warehouse** - Separate OLAP system (Redshift, BigQuery) for P&L analysis, risk reporting without impacting production.

### Performance Optimization:

**Connection Pooling** - Essential for handling thousands of concurrent orders, reuse connections to reduce overhead.

**Prepared Statements** - Prevent SQL injection, pre-compiled for faster execution, critical for security.

**Query Optimization** - EXPLAIN ANALYZE queries to ensure index usage for order lookups, avoid table scans on large tables.

**Caching Strategy** - Redis for instrument data, order status cache, position snapshots.

**Materialized Views** - Pre-computed P&L, risk metrics refreshed periodically for dashboard performance.

### Data Quality & Compliance:

**Audit Logging** - Immutable logs of all trades for regulatory compliance and dispute resolution.

**Master Data Management** - Single source of truth for instruments, accounts, counterparties.

**Data Governance** - Access control (traders see their orders only), data retention policies (historical data archival).

**Backup & Recovery** - Critical for 24/7 trading: near-real-time replication, tested recovery procedures.

All concepts include practical context specifically relevant to systems design, performance-critical applications, and financial infrastructure!
