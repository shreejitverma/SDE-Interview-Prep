# Complete Database Management Systems Reference - 199 Keywords

## 1. CORE DBMS CONCEPTS (23 KEYWORDS)

### Fundamental Concepts (15)
1. Database Management System (DBMS) - Software managing data definition, storage, querying
2. Database - Organized collection of related data stored persistently
3. Data Model - Formal representation of data structure
4. Schema - Blueprint defining database organization
5. Relation - Two-dimensional table (rows and columns)
6. Tuple - Single row/record in table
7. Attribute - Column in relation
8. Entity - Thing/concept represented in database
9. Relationship - Association between entities
10. Primary Key - Unique identifier for each row
11. Foreign Key - Reference to primary key in another table
12. Index - Data structure speeding up retrieval
13. Query - Request for data retrieval/manipulation
14. Transaction - Logical unit of work (all-or-nothing)
15. ACID Properties - Atomicity, Consistency, Isolation, Durability

### Relational Model (8)
16. Relational Model - Data organized in tables
17. Normalization - Organizing data to minimize redundancy
18. First Normal Form (1NF) - Eliminate repeating groups
19. Second Normal Form (2NF) - Remove partial dependencies
20. Third Normal Form (3NF) - Remove transitive dependencies
21. Boyce-Codd Normal Form (BCNF) - Strictest normal form
22. Denormalization - Adding redundancy for performance
23. Cardinality - Relationship type (1:1, 1:N, M:N)

## 2. DATA MANAGEMENT & QUERIES (24 KEYWORDS)

### SQL Fundamentals (15)
24. SQL - Structured Query Language standard
25. DDL - Data Definition Language
26. DML - Data Manipulation Language
27. DCL - Data Control Language
28. TCL - Transaction Control Language
29. SELECT Statement - Query retrieving data
30. JOIN - Combining rows from multiple tables
31. INNER JOIN - Matching rows from both tables
32. LEFT JOIN - All left rows plus matching right
33. GROUP BY - Aggregating rows into groups
34. HAVING - Filtering groups after aggregation
35. ORDER BY - Sorting query results
36. Subquery - Query nested inside another
37. Aggregate Functions - SUM, COUNT, AVG, MIN, MAX
38. UNION - Combining results from multiple SELECT

### Data Modification (9)
39. INSERT - Adding new rows
40. UPDATE - Modifying existing rows
41. DELETE - Removing rows
42. TRUNCATE - Fast removal of all rows
43. Constraint - Rule enforcing data integrity
44. UNIQUE Constraint - Ensures column uniqueness
45. CHECK Constraint - Validates condition
46. NOT NULL Constraint - Ensures value presence
47. DEFAULT - Default value on insert

## 3. DATABASE ARCHITECTURE (21 KEYWORDS)

### Core Components (10)
48. Query Processor - Parses, validates, optimizes queries
49. Query Optimizer - Determines efficient execution plan
50. Execution Engine - Executes optimized query plan
51. Storage Engine - Manages physical data storage
52. Buffer Manager - Caches frequently accessed data
53. Transaction Manager - Manages transaction execution
54. Recovery Manager - Restores database after failures
55. Concurrency Control Manager - Manages concurrent access
56. Security Manager - Controls user access
57. Catalog/Data Dictionary - Metadata repository

### Storage Structures (11)
58. File Organization - How records organized on disk
59. Heap File - Unordered record storage
60. Sorted File - Records ordered by key
61. Hashed File - Records distributed by hash
62. Block - Basic storage unit on disk
63. Record - Logical data unit
64. Page - In-memory disk block copy
65. Extent - Contiguous blocks allocated
66. B-Tree Index - Balanced tree for searching
67. Hash Index - Hash table for point lookups
68. Bitmap Index - Bit array for low-cardinality

## 4. TRANSACTION MANAGEMENT (21 KEYWORDS)

### ACID Properties (8)
69. Atomicity - All-or-nothing execution
70. Consistency - Valid state to valid state
71. Isolation - Concurrent independence
72. Durability - Persistence after commit
73. Dirty Read - Reading uncommitted data
74. Non-Repeatable Read - Different results on re-read
75. Phantom Read - Range query varies
76. Lost Update - Concurrent write conflict

### Isolation Levels (5)
77. Read Uncommitted - Lowest isolation (dirty reads)
78. Read Committed - Standard isolation
79. Repeatable Read - Snapshot consistency
80. Serializable - Highest isolation
81. Snapshot Isolation - MVCC-based

### Concurrency Control (8)
82. Pessimistic Locking - Acquire locks before access
83. Optimistic Locking - Check conflicts at commit
84. Read Lock (Shared Lock) - Multiple readers
85. Write Lock (Exclusive Lock) - Single writer
86. Deadlock - Circular wait
87. Lock Timeout - Timeout waiting
88. Two-Phase Locking (2PL) - Growing then shrinking
89. MVCC - Multiple-Version Concurrency Control

## 5. INDEXING & PERFORMANCE (19 KEYWORDS)

### Index Types (9)
90. Primary Index - Index on primary key
91. Secondary Index - Index on non-key column
92. Clustered Index - Determines record order
93. Non-Clustered Index - Separate structure
94. Composite Index - Index on multiple columns
95. Unique Index - Ensures uniqueness
96. Full-Text Index - Text searching
97. Spatial Index - Geometric data
98. Partial Index - Index on subset

### Query Optimization (10)
99. Query Plan - Sequence of operations
100. Cost Model - Estimates execution cost
101. Selectivity - Fraction of rows matching
102. Cardinality - Distinct values count
103. Join Order - Sequence of joining tables
104. Index Scan - Reading index sequentially
105. Index Seek - Direct index access
106. Table Scan - Reading entire table
107. EXPLAIN/ANALYZE - Shows query plan
108. Execution Plan Visualization - Graphical plan

## 6. DATABASE TYPES (17 KEYWORDS)

### Traditional Databases (4)
109. Relational Database (RDBMS) - Tables with schema
110. SQL Database - Using SQL queries
111. OLTP - Online Transaction Processing
112. OLAP - Online Analytical Processing

### NoSQL Databases (7)
113. NoSQL Database - Non-relational
114. Document Database - JSON/BSON documents
115. Key-Value Store - Simple map
116. Wide-Column Store - Column families
117. Graph Database - Nodes and edges
118. Search Engine - Full-text search
119. Time-Series Database - Timestamped data

### Specialized Databases (6)
120. NewSQL Database - SQL with scalability
121. Columnar Database - Column-wise storage
122. In-Memory Database - Data in RAM
123. Distributed Database - Multi-location
124. Data Warehouse - Historical data
125. Data Lake - Raw data repository

## 7. DATA INTEGRITY (12 KEYWORDS)

### Constraints & Validation (8)
126. Data Integrity - Accuracy and consistency
127. Entity Integrity - Unique identifiers
128. Referential Integrity - Valid foreign keys
129. Domain Integrity - Valid value range
130. User-Defined Integrity - Custom rules
131. Constraint - Data rule
132. Trigger - Automated action on event
133. Stored Procedure - Pre-compiled SQL code

### Data Quality (4)
134. Data Validation - Pre-insertion checking
135. Data Cleansing - Correcting invalid data
136. Master Data Management - Central data truth
137. Data Governance - Quality policies

## 8. ADVANCED DBMS FEATURES (13 KEYWORDS)

### Partitioning & Distribution (8)
138. Partitioning - Dividing table into parts
139. Range Partitioning - Partition by value range
140. Hash Partitioning - Even distribution
141. List Partitioning - Predefined lists
142. Sharding - Horizontal data splitting
143. Replication - Data copying
144. Master-Slave Replication - Single master
145. Multi-Master Replication - Multiple masters

### Performance Features (5)
146. Query Caching - Storing query results
147. Connection Pooling - Reusing connections
148. Prepared Statements - Pre-compiled queries
149. Batch Processing - Multiple operations
150. Materialized View - Pre-computed results

## 9. SECURITY & BACKUP (17 KEYWORDS)

### Access Control (7)
151. Authentication - Verifying identity
152. Authorization - Controlling access
153. Role-Based Access Control (RBAC) - User roles
154. User Account - Login credential
155. Privilege - Operation permission
156. Grant - Giving privileges
157. Revoke - Removing privileges

### Data Protection (10)
158. Encryption - Encoding data
159. Encryption At-Rest - Storage encoding
160. Encryption In-Transit - Network encoding
161. Hashing - One-way transformation
162. Backup - Database copy
163. Full Backup - Complete copy
164. Incremental Backup - Changes only
165. Differential Backup - Changes since full
166. Recovery - Restoring from backup
167. Point-in-Time Recovery - Specific moment

## 10. ADMINISTRATION (14 KEYWORDS)

### Database Administration (9)
168. Database Administrator (DBA) - System manager
169. Monitoring - Performance tracking
170. Tuning - Performance optimization
171. Maintenance - Health tasks
172. Indexing Strategy - Index design
173. Table Statistics - Data distribution info
174. Query Profiling - Performance analysis
175. Capacity Planning - Resource estimation
176. Upgrade Management - Version updates

### Database Maintenance (5)
177. Vacuum - Dead row cleanup
178. Analyze - Update statistics
179. Defragmentation - Reorganize data
180. Consistency Check - Integrity verification
181. Log Rotation - Archive transaction logs

## 11. ADVANCED TOPICS (18 KEYWORDS)

### Distributed Systems (7)
182. Distributed Database - Multi-location data
183. CAP Theorem - Consistency/Availability/Partition tradeoff
184. Consistency Model - Data visibility guarantees
185. Eventual Consistency - Temporary inconsistency
186. Strong Consistency - Immediate visibility
187. Consensus Algorithm - Distributed agreement
188. Quorum - Majority agreement

### Query Processing (6)
189. Query Parser - Syntax validation
190. Query Translator - SQL to internal format
191. Query Optimizer - Execution plan selection
192. Query Execution - Plan execution
193. Explain Plan - Query execution display
194. Execution Statistics - Runtime information

### Advanced Indexing (5)
195. Covering Index - All columns in index
196. Index Selectivity - Filtering effectiveness
197. Index Cardinality - Distinct values
198. Redundant Index - Unnecessary index
199. Index Maintenance - Index optimization

## COMPLETE STATISTICS

**Total DBMS Keywords: 199**

Breakdown:
- Core DBMS Concepts: 23
- Data Management & Queries: 24
- Database Architecture: 21
- Transaction Management: 21
- Indexing & Performance: 19
- Database Types: 17
- Data Integrity: 12
- Advanced Features: 13
- Security & Backup: 17
- Administration: 14
- Advanced Topics: 18

All organized for comprehensive database understanding.
