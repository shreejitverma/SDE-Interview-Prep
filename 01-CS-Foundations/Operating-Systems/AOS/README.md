---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: [https://omscs.gatech.edu/cs-6210-advanced-operating-systems]
course: cs6210
tags: [cs6210]
aliases: [CS 6210, AOS, Advanced Operating Systems]
cssclasses: [wide-page]
---

# Georgia Tech CS 6210 - Advanced Operating Systems

Graduate operating systems as taught by Prof. Umakishore Ramachandran: OS structure, virtualization, parallel systems, distributed systems, distributed objects and middleware, distributed subsystems, failures and recovery, internet-scale computing, real-time and multimedia, and security.
The course traces the ideas inside today's systems back to the research papers that introduced them, so every lesson here has notes, paper notes, a hands-on lab, and practice questions.

> [!info] How this section is organized
> - **Lesson notes** in five Parts plus an optional refresher, one note per sub-lesson, one `###` heading per concept.
> - **[Papers](Papers/README.md)**: one note per paper on the reading list, with reading status from the syllabus.
> - **[Labs](labs/README.md)**: real commands, C and Python programs, and measurements in a Linux VM ([setup](labs/setup/README.md)).
> - **[Practice](Practice/README.md)**: original exam-style questions with folded answers, and the master **[Exam Practice Set](Exam-Prep/Exam-Practice-Questions.md)**.
> - **[Study plan](00-Study-Plan.md)** (semester and two-week crash plans), **[resources](00-Resources.md)**, and the **[coverage report](00-Coverage.md)**.
> - [Cheat sheets](Cheatsheets/README.md) per Part.
> - Views: the [study board](AOS-Study-Board.md) (Kanban), the [concept map](AOS-Concept-Map.canvas) (Canvas), and the [notes base](AOS-Notes.base) (Bases).

> [!warning] Honor code
> This vault is public.
> It contains original notes, original practice questions, and labs that are not course project solutions.
> Project specifications, project code, past exams, review-question answers, slides, and paper PDFs are never committed here; course material stays in the private library.

## Learning outcomes

After this course you should be able to:

1. Explain how OS structure trades off protection, performance, and extensibility, and compare monolithic, microkernel, SPIN, Exokernel, and L3 designs with numbers for border-crossing costs.
2. Explain how a hypervisor virtualizes memory (shadow and nested paging, ballooning, page sharing), CPU, and devices, and contrast full and para-virtualization.
3. Reason about cache coherence and memory consistency, and design and analyze scalable spinlocks and barriers by their bus traffic, latency, and contention.
4. Explain lightweight RPC, multiprocessor scheduling with cache affinity, and OS structures that scale on many cores (clustered objects, Corey, Cellular Disco).
5. Order events in a distributed system with logical clocks, implement Lamport mutual exclusion, and analyze where RPC latency goes on a LAN.
6. Explain active networks, building systems from verified components, and distributed object systems (Spring, Java RMI, EJB) and their design alternatives.
7. Explain global memory systems, software distributed shared memory with lazy release consistency, and serverless distributed file systems.
8. Build and reason about recoverable virtual memory and transactional recovery (LRVM, RioVista, Quicksilver, System R).
9. Reason about giant-scale services (DQ principle, graceful degradation, online evolution), MapReduce, and DHT-based content delivery (Coral, Dynamo).
10. Explain OS support for time-sensitive applications and live streams (TS-Linux, PTS), and apply protection principles to a distributed system (Saltzer and Schroeder, Andrew).

## Course map

```mermaid
flowchart LR
  R[Refresher] --> L1[L01 Intro]
  L1 --> L2[L02 OS Structure]
  L2 --> L3[L03 Virtualization]
  L3 --> L4[L04 Parallel Systems]
  L4 --> L5[L05 Distributed Systems]
  L5 --> L6[L06 Distributed Objects]
  L6 --> L7[L07 Distributed Subsystems]
  L7 --> L8[L08 Failures and Recovery]
  L8 --> L9[L09 Internet Computing]
  L9 --> L10[L10 RT and Multimedia]
  L10 --> L11[L11 Security]
```

| Part | Lessons | Test (Fall 2026) |
| --- | --- | --- |
| [Part 0 - Refresher](Part-0-Refresher/README.md) | Optional background | - |
| [Part 1 - OS Structure and Virtualization](Part-1-OS-Structure-and-Virtualization/README.md) | L01-L03 | Test 1 |
| [Part 2 - Parallel Systems](Part-2-Parallel-Systems/README.md) | L04 | Test 1 |
| [Part 3 - Distributed Systems](Part-3-Distributed-Systems/README.md) | L05-L06 | Test 2 |
| [Part 4 - Distributed Subsystems and Recovery](Part-4-Distributed-Subsystems-and-Recovery/README.md) | L07-L08 | Test 2 (L07), Test 3 (L08) |
| [Part 5 - Internet Scale, Real Time, and Security](Part-5-Internet-Scale-Real-Time-and-Security/README.md) | L09-L11 | Test 3 |

## Progress

Lesson notes by status (needs the Dataview plugin):

```dataview
TABLE WITHOUT ID file.link AS Note, sub_lesson AS Lesson, status AS Status, last_reviewed AS Reviewed
FROM #cs6210/lesson
SORT sub_lesson ASC
```

Papers by lesson and reading status:

```dataview
TABLE WITHOUT ID file.link AS Paper, lesson AS Lesson, reading AS Reading, status AS Status
FROM #cs6210/paper
SORT lesson ASC
```

Open study tasks this week and overdue (needs the Tasks plugin):

```tasks
not done
path includes AOS/00-Study-Plan
limit 15
```

## Quick links

- [Coverage report](00-Coverage.md): concepts covered per lesson, generated by `python3 tools/check_aos_coverage.py --write-report`.
- [Labs setup](labs/setup/README.md): the Lima VM and package list.
- Earlier course: [GIOS (CS 6200)](../GIOS/_GIOS-Dashboard.md).
- Templates (Templater): `CS6210-Lesson`, `CS6210-Paper`, and `CS6210-Lab` in `16-Interview-Command-Center/_Templates`.
