# Vault Audit

Generated 2026-09-21 by `python3 tools/audit_vault.py` over git-tracked files.
Re-run the script after every structural change; this file is its output and should not be hand-edited.
0 files in private locations (`tools/private_paths.py`) are excluded; `tools/audit_pii.py` inventories them into a private path.

## Summary

| Check | Result |
| :--- | ---: |
| Tracked files | 15716 |
| Markdown notes | 1516 |
| Internal links checked | 9389 |
| Broken links (links into private locations are not counted) | 0 |
| Wikilink aliases that split a table cell | 0 |
| Broken wikilinks fixable by unique basename | 0 |
| Orphan knowledge notes (no inbound links) | 3 |
| Archived drafts (`_archive/`, `_consolidated*/`) | 305 |
| Knowledge notes without frontmatter | 0 |
| Note folders without README (depth <= 3) | 0 |
| Notes with emojis / total emojis | 0 / 0 |
| Notes with em dashes / total em dashes | 0 / 0 |
| Identical-content groups / redundant MB | 418 / 236.5 |
| Vendored or imported repos | 11 |
| Tracked build junk | 64 |
| Files >= 5 MB / total MB | 54 / 857.0 |

## 1. File counts by top-level folder

| Folder | Files | Notes | Code | Papers | Other | MB | Last touched |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `(root)` | 11 | 6 | 0 | 0 | 5 | 0.1 | 2026-09-21 |
| `.github` | 1 | 0 | 0 | 0 | 1 | 0.0 | 2026-09-21 |
| `.idea` | 6 | 0 | 0 | 0 | 6 | 0.0 | 2024-04-01 |
| `.obsidian` | 100 | 0 | 30 | 0 | 70 | 46.2 | 2026-09-21 |
| `.vscode` | 5 | 0 | 0 | 0 | 5 | 0.0 | 2024-04-01 |
| `00-Start-Here` | 3 | 3 | 0 | 0 | 0 | 0.0 | 2026-09-21 |
| `01-CS-Foundations` | 211 | 65 | 59 | 57 | 30 | 139.7 | 2026-09-21 |
| `02-Programming-Languages` | 5866 | 798 | 2856 | 482 | 1730 | 422.7 | 2026-09-21 |
| `03-Data-Structures-Algorithms` | 7749 | 82 | 7506 | 27 | 134 | 29.5 | 2026-09-21 |
| `04-System-Design` | 1237 | 67 | 719 | 142 | 309 | 706.2 | 2026-09-21 |
| `05-Quantitative-Finance` | 9 | 1 | 7 | 1 | 0 | 12.1 | 2026-09-21 |
| `06-Interview-Prep` | 4 | 4 | 0 | 0 | 0 | 0.0 | 2026-09-21 |
| `07-Project-Portfolio` | 1 | 1 | 0 | 0 | 0 | 0.0 | 2026-09-21 |
| `08-Distinguished-Engineering` | 9 | 3 | 6 | 0 | 0 | 0.0 | 2026-09-21 |
| `09-Engineering-Leadership` | 3 | 3 | 0 | 0 | 0 | 0.0 | 2026-09-21 |
| `10-Development-Practices` | 4 | 2 | 0 | 0 | 2 | 0.0 | 2026-09-21 |
| `11-Security-And-Cryptography` | 3 | 2 | 1 | 0 | 0 | 0.0 | 2026-09-21 |
| `12-Performance-Engineering` | 4 | 2 | 2 | 0 | 0 | 0.0 | 2026-09-21 |
| `13-Agentic-AI` | 122 | 122 | 0 | 0 | 0 | 2.3 | 2026-09-21 |
| `14-Low-Latency-Systems` | 262 | 262 | 0 | 0 | 0 | 1.7 | 2026-09-21 |
| `15-Technical-Whitepapers` | 44 | 44 | 0 | 0 | 0 | 0.3 | 2026-09-21 |
| `16-Interview-Command-Center` | 47 | 47 | 0 | 0 | 0 | 0.1 | 2026-09-21 |
| `CS-Subjects` | 1 | 1 | 0 | 0 | 0 | 0.0 | 2026-09-21 |
| `tools` | 14 | 1 | 11 | 0 | 2 | 0.1 | 2026-09-21 |

## 2. Duplicate and overlapping sections

### Folders whose names normalize to the same topic

- **design pattern**: `04-System-Design/03-Design-Patterns`, `04-System-Design/Design Patterns`, `04-System-Design/design-patterns-java`, `04-System-Design/design-patterns-python`, `04-System-Design/python-design-patterns`
- **godhood to zero**: `02-Programming-Languages/C++/CPP_Zero_to_Godhood`, `02-Programming-Languages/Python/Python_Zero_to_Godhood`
- **linked list**: `02-Programming-Languages/C++/Linked List`, `03-Data-Structures-Algorithms/01-Topics/Linked-Lists`
- **recursion**: `02-Programming-Languages/C++/Recursion`, `03-Data-Structures-Algorithms/01-Topics/Recursion`
- **sorting**: `02-Programming-Languages/C++/Sorting`, `03-Data-Structures-Algorithms/01-Topics/Sorting`
- **advanced concurrency**: `02-Programming-Languages/Java/02-Advanced-Concurrency`, `08-Distinguished-Engineering/01-Advanced-Concurrency`
- **dynamic programming**: `03-Data-Structures-Algorithms/01-Topics/Dynamic-Programming`, `03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns/Dynamic-Programming`
- **graph**: `03-Data-Structures-Algorithms/01-Topics/Graphs`, `03-Data-Structures-Algorithms/04-Gold-Standard-Cpp-Patterns/Graphs`
- **mathematic**: `03-Data-Structures-Algorithms/01-Topics/Mathematics`, `05-Quantitative-Finance/01-Mathematics`
- **limiter rate**: `04-System-Design/02-Case-Studies/02-Rate-Limiter`, `04-System-Design/Low Level Design/rate-limiter`
- **behavioral**: `04-System-Design/03-Design-Patterns/Behavioral`, `06-Interview-Prep/01-Behavioral`
- **dev quant**: `05-Quantitative-Finance/02-Quant-Dev`, `16-Interview-Command-Center/01-Roles/Quant-Dev`

### Folder pairs sharing the most identical files

| Folder A | Folder B | Identical files |
| :--- | ---: | ---: |
| `04-System-Design/Design Patterns` | `04-System-Design/python-design-patterns` | 92 |
| `04-System-Design/InterviewReady` | `04-System-Design/Low Level Design` | 3 |
| `01-CS-Foundations/Object-Oriented-Programming` | `04-System-Design/design-questions` | 1 |
| `04-System-Design/InterviewReady` | `04-System-Design/design-questions` | 1 |
| `04-System-Design/Low Level Design` | `04-System-Design/design-questions` | 1 |

### Largest identical-content groups (redundant bytes)

| First copy | Copies | Redundant MB |
| :--- | ---: | ---: |
| `04-System-Design/ByteByteGo/ByteByteGo_The_Big_Archive_1652841223 2022-05-18 02_33_51.pdf` | 2 | 39.5 |
| `02-Programming-Languages/Python/python in depth/Django_Blog/09-Update-User-Profile/django_project/media/profile_pics/large.jpg` | 14 | 35.3 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1493976040374-85c8e12f0c0e.jpg` | 2 | 21.0 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1541698444083-023c97d3f4b6.jpg` | 2 | 17.1 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1532009324734-20a7a5813719.jpg` | 2 | 14.9 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1522364723953-452d3431c267.jpg` | 2 | 12.9 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1513938709626-033611b8cc03.jpg` | 2 | 12.1 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1524429656589-6633a470097c.jpg` | 2 | 11.5 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1550439062-609e1531270e.jpg` | 2 | 10.9 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1530122037265-a5f1f91d3b99.jpg` | 2 | 10.5 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1530224264768-7ff8c1789d79.jpg` | 2 | 10.0 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1504198453319-5ce911bafcde.jpg` | 2 | 8.8 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1516972810927-80185027ca84.jpg` | 2 | 7.9 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1549692520-acc6669e2f0c.jpg` | 2 | 4.0 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1564135624576-c5c88640f235.jpg` | 2 | 3.6 |

### Folder pairs with overlapping note or code names (topical overlap)

| Folder A | Folder B | Shared names | Overlap of smaller |
| :--- | ---: | ---: | ---: |
| `04-System-Design/Design Patterns` | `04-System-Design/python-design-patterns` | 57 | 100% |
| `03-Data-Structures-Algorithms/02-Practice-Platforms` | `04-System-Design/Most Asked Design Questions` | 50 | 100% |
| `04-System-Design/design-patterns-java` | `04-System-Design/design-questions` | 59 | 44% |
| `03-Data-Structures-Algorithms/01-Topics` | `03-Data-Structures-Algorithms/02-Practice-Platforms` | 103 | 11% |
| `02-Programming-Languages/Python` | `03-Data-Structures-Algorithms/01-Topics` | 82 | 9% |
| `02-Programming-Languages/C++` | `03-Data-Structures-Algorithms/01-Topics` | 16 | 3% |
| `02-Programming-Languages/C++` | `02-Programming-Languages/Python` | 11 | 2% |
| `02-Programming-Languages/Python` | `03-Data-Structures-Algorithms/02-Practice-Platforms` | 16 | 1% |

## 3. Broken links

| Folder | Broken |
| :--- | ---: |

0 broken wikilinks point at a path that no longer exists but name a note that exists exactly once elsewhere.
These come from folder reorganizations that did not rewrite links and can be fixed mechanically.

<details>
<summary>All 0 broken links</summary>


</details>

## 4. Orphan notes

Notes that no other note links to. Most become reachable once each folder has a Map of Content.

| Folder | Orphans |
| :--- | ---: |
| `07-Project-Portfolio` | 1 |
| `08-Distinguished-Engineering` | 1 |
| `CS-Subjects` | 1 |

<details>
<summary>All 3 orphans</summary>

- `07-Project-Portfolio/README.md`
- `08-Distinguished-Engineering/README.md`
- `CS-Subjects/README.md`

</details>

## 5. Notes without frontmatter

| Folder | Notes |
| :--- | ---: |

<details>
<summary>All 0 notes</summary>


</details>

## 6. Note folders without a README

A folder counts as covered by `README.md`, `_README.md`, `index.md`, a folder note named after it, `00 Home`, `00-Dashboard`, or a `MOC - ` note.

<details>
<summary>All 0 folders</summary>


</details>

## 7. Emoji and em-dash violations

| Note | Emojis |
| :--- | ---: |

<details>
<summary>All 0 notes with emojis</summary>


</details>

| Note | Em dashes |
| :--- | ---: |

<details>
<summary>All 0 notes with em dashes</summary>


</details>

## 8. Stale and scraped content

### Vendored or imported repos

Folders carrying their own LICENSE, `.gitignore`, `package.json`, or similar; outermost only.

| Folder | Files | Last touched |
| :--- | ---: | ---: |
| `03-Data-Structures-Algorithms/02-Practice-Platforms/LeetCode` | 5953 | 2026-09-21 |
| `02-Programming-Languages/Python/python in depth` | 2075 | 2026-09-21 |
| `04-System-Design/design-patterns-java` | 193 | 2026-09-21 |
| `04-System-Design/design-questions` | 179 | 2026-09-21 |
| `04-System-Design/Low Level Design` | 137 | 2026-09-21 |
| `04-System-Design/Design Patterns/python-patterns` | 106 | 2026-09-21 |
| `04-System-Design/python-design-patterns` | 106 | 2026-09-21 |
| `03-Data-Structures-Algorithms/01-Topics/General-DSA` | 94 | 2026-09-21 |
| `04-System-Design/InterviewReady/splitwise` | 31 | 2026-07-24 |
| `01-CS-Foundations/Operating-Systems/code/os` | 20 | 2026-07-24 |
| `01-CS-Foundations/Object-Oriented-Programming/code/oop` | 16 | 2026-07-24 |

### Large files

Files of 5 MB or more; candidates for Git LFS, external links, or removal.

| File | MB |
| :--- | ---: |
| `04-System-Design/OOPs_Object_Oriented_Programming_by_Kapil_Yadav.pdf` | 90.8 |
| `04-System-Design/Microsoft_Design_Challenge.pdf` | 56.9 |
| `04-System-Design/System_Design_Handwritten_Notes_by_Aman_Barnwal.pdf` | 49.0 |
| `04-System-Design/ByteByteGo/System_Design_1659383261 2022-08-01 19_47_52.pdf` | 45.2 |
| `04-System-Design/ByteByteGo/System_Design_Interview_Prep_Notes_Revanth_Murigipudi_1651502215 2022-05-02 14_37_07.pdf` | 41.8 |
| `04-System-Design/ByteByteGo/System_Design_The_Big_Archive_1655113375 2022-06-13 10_27_48.pdf` | 39.5 |
| `04-System-Design/ByteByteGo/ByteByteGo_The_Big_Archive_1652841223 2022-05-18 02_33_51.pdf` | 39.5 |
| `04-System-Design/dive-into-design-patterns.pdf` | 34.0 |
| `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1493976040374-85c8e12f0c0e.jpg` | 21.0 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1493976040374-85c8e12f0c0e.jpg` | 21.0 |
| `04-System-Design/Advanced.Programming.in.the.UNIX.Environment.3rd.Edition.pdf` | 20.6 |
| `04-System-Design/InterviewReady/Prototyping/The-Beginners-Guide-to-Rapid-Prototyping.pdf` | 19.2 |
| `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1541698444083-023c97d3f4b6.jpg` | 17.1 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1541698444083-023c97d3f4b6.jpg` | 17.1 |
| `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1532009324734-20a7a5813719.jpg` | 14.9 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1532009324734-20a7a5813719.jpg` | 14.9 |
| `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1522364723953-452d3431c267.jpg` | 12.9 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1522364723953-452d3431c267.jpg` | 12.9 |
| `04-System-Design/System_Design.pdf` | 12.2 |
| `05-Quantitative-Finance/01-Mathematics/A Practical Guide To Quantitative Finance Interviews by Xinfeng Zhou (z-lib.org).pdf` | 12.1 |
| `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1513938709626-033611b8cc03.jpg` | 12.1 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1513938709626-033611b8cc03.jpg` | 12.1 |
| `04-System-Design/System_Design_Handbook_Aman_Barnwal.pdf` | 11.8 |
| `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1524429656589-6633a470097c.jpg` | 11.5 |
| `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1524429656589-6633a470097c.jpg` | 11.5 |

<details>
<summary>All 54 large files</summary>

- `04-System-Design/OOPs_Object_Oriented_Programming_by_Kapil_Yadav.pdf`: 90.8 MB
- `04-System-Design/Microsoft_Design_Challenge.pdf`: 56.9 MB
- `04-System-Design/System_Design_Handwritten_Notes_by_Aman_Barnwal.pdf`: 49.0 MB
- `04-System-Design/ByteByteGo/System_Design_1659383261 2022-08-01 19_47_52.pdf`: 45.2 MB
- `04-System-Design/ByteByteGo/System_Design_Interview_Prep_Notes_Revanth_Murigipudi_1651502215 2022-05-02 14_37_07.pdf`: 41.8 MB
- `04-System-Design/ByteByteGo/System_Design_The_Big_Archive_1655113375 2022-06-13 10_27_48.pdf`: 39.5 MB
- `04-System-Design/ByteByteGo/ByteByteGo_The_Big_Archive_1652841223 2022-05-18 02_33_51.pdf`: 39.5 MB
- `04-System-Design/dive-into-design-patterns.pdf`: 34.0 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1493976040374-85c8e12f0c0e.jpg`: 21.0 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1493976040374-85c8e12f0c0e.jpg`: 21.0 MB
- `04-System-Design/Advanced.Programming.in.the.UNIX.Environment.3rd.Edition.pdf`: 20.6 MB
- `04-System-Design/InterviewReady/Prototyping/The-Beginners-Guide-to-Rapid-Prototyping.pdf`: 19.2 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1541698444083-023c97d3f4b6.jpg`: 17.1 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1541698444083-023c97d3f4b6.jpg`: 17.1 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1532009324734-20a7a5813719.jpg`: 14.9 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1532009324734-20a7a5813719.jpg`: 14.9 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1522364723953-452d3431c267.jpg`: 12.9 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1522364723953-452d3431c267.jpg`: 12.9 MB
- `04-System-Design/System_Design.pdf`: 12.2 MB
- `05-Quantitative-Finance/01-Mathematics/A Practical Guide To Quantitative Finance Interviews by Xinfeng Zhou (z-lib.org).pdf`: 12.1 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1513938709626-033611b8cc03.jpg`: 12.1 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1513938709626-033611b8cc03.jpg`: 12.1 MB
- `04-System-Design/System_Design_Handbook_Aman_Barnwal.pdf`: 11.8 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1524429656589-6633a470097c.jpg`: 11.5 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1524429656589-6633a470097c.jpg`: 11.5 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1550439062-609e1531270e.jpg`: 10.9 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1550439062-609e1531270e.jpg`: 10.9 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1530122037265-a5f1f91d3b99.jpg`: 10.5 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1530122037265-a5f1f91d3b99.jpg`: 10.5 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1530224264768-7ff8c1789d79.jpg`: 10.0 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1530224264768-7ff8c1789d79.jpg`: 10.0 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1504198453319-5ce911bafcde.jpg`: 8.8 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1504198453319-5ce911bafcde.jpg`: 8.8 MB
- `02-Programming-Languages/Python/python in depth/Python/Threading/photo-1516972810927-80185027ca84.jpg`: 7.9 MB
- `02-Programming-Languages/Python/python in depth/Python/MultiProcessing/photo-1516972810927-80185027ca84.jpg`: 7.9 MB
- `04-System-Design/design-patterns-java/notes/04-abstract-factory-adapter-hw.pdf`: 7.2 MB
- `01-CS-Foundations/Operating-Systems/notes/04-memory-management-hw.pdf`: 7.1 MB
- `01-CS-Foundations/Computer-Networks/notes/03-cookies-dns-tcp-hw.pdf`: 6.5 MB
- `01-CS-Foundations/Object-Oriented-Programming/notes/03-polymorphism-hw.pdf`: 6.3 MB
- `04-System-Design/design-patterns-java/notes/07-facade-observer-hw.pdf`: 6.1 MB
- `04-System-Design/SysDesign-GauravSen.pdf`: 6.1 MB
- `04-System-Design/System_Design_Introduction_and_Roadmap.pdf`: 6.0 MB
- `04-System-Design/design-patterns-java/notes/08-strategy-uml-hw.pdf`: 5.9 MB
- `04-System-Design/design-patterns-java/notes/01-singleton-builder-hw.pdf`: 5.7 MB
- `01-CS-Foundations/DBMS/notes/02-integrity-er-diagram-hw.pdf`: 5.6 MB
- `01-CS-Foundations/DBMS/notes/02-schema-design-hw.pdf`: 5.6 MB
- `01-CS-Foundations/DBMS/notes/04-transactions-indexes-hw-03.pdf`: 5.4 MB
- `.obsidian/plugins/copilot/main.js`: 5.3 MB
- `.obsidian/plugins/tasknotes/main.js`: 5.2 MB
- `01-CS-Foundations/DBMS/notes/02-integrity-er-diagram-hw-02.pdf`: 5.2 MB
- `02-Programming-Languages/C++/CPlusPlusNotesForProfessionals.pdf`: 5.1 MB
- `.obsidian/plugins/obsidian-excalidraw-plugin/main.js`: 5.1 MB
- `.obsidian/plugins/realclaudian/main.js`: 5.1 MB
- `01-CS-Foundations/DBMS/notes/03-normalisation-acid-hw.pdf`: 5.0 MB

</details>

### Tracked build junk

`.DS_Store`, CMake build trees, object files, and similar that should be gitignored.

| Folder | Files |
| :--- | ---: |
| `04-System-Design` | 61 |
| `01-CS-Foundations` | 3 |

<details>
<summary>All 64 junk files</summary>

- `01-CS-Foundations/Operating-Systems/code/os/target/classes/com/scaler/App.class`
- `01-CS-Foundations/Operating-Systems/code/os/target/classes/com/scaler/producerconsumer/UnitOfWork.class`
- `01-CS-Foundations/Operating-Systems/code/os/target/test-classes/com/scaler/AppTest.class`
- `04-System-Design/Design Patterns/BehaviroalPatterns/observer/observer_example.o`
- `04-System-Design/Design Patterns/CreationalPatterns/factory-method/bike.o`
- `04-System-Design/Design Patterns/CreationalPatterns/factory-method/car.o`
- `04-System-Design/Design Patterns/CreationalPatterns/factory-method/client.o`
- `04-System-Design/Design Patterns/CreationalPatterns/factory-method/smart_client.o`
- `04-System-Design/Design Patterns/CreationalPatterns/factory-method/vehicle_factory.o`
- `04-System-Design/InterviewReady/splitwise/target/classes/Splitwise.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/Amount.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/BalanceMap.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/Currency.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/Expense.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/Group.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/PaymentGraph.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/models/User.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/services/ExpenseService.class`
- `04-System-Design/InterviewReady/splitwise/target/classes/services/GroupService.class`
- `04-System-Design/InterviewReady/splitwise/target/test-classes/GroupPaymentGraphTest.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/Cache.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/CacheBuilder.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/DataSource.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/events/Event.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/events/Eviction$Type.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/events/Eviction.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/events/Load.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/events/Update.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/events/Write.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/models/AccessDetails.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/models/EvictionAlgorithm.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/models/FetchAlgorithm.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/models/Record.class`
- `04-System-Design/Low Level Design/distributed-cache/target/classes/models/Timer.class`
- `04-System-Design/Low Level Design/distributed-cache/target/test-classes/TestCache$1.class`
- `04-System-Design/Low Level Design/distributed-cache/target/test-classes/TestCache$2.class`
- `04-System-Design/Low Level Design/distributed-cache/target/test-classes/TestCache.class`
- `04-System-Design/Low Level Design/distributed-cache/target/test-classes/models/SettableTimer.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/EventBus.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/exceptions/RetryLimitExceededException.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/exceptions/UnsubscribedPollException.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/lib/KeyedExecutor.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/models/Event.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/models/EventType.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/models/FailureEvent.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/models/Subscription.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/classes/util/Timer.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/test-classes/EventBusTest.class`
- `04-System-Design/Low Level Design/distributed-event-bus/target/test-classes/TestTimer.class`
- `04-System-Design/Low Level Design/rate-limiter/target/classes/TimerWheel.class`
- `04-System-Design/Low Level Design/rate-limiter/target/classes/exceptions/RateLimitExceededException.class`
- `04-System-Design/Low Level Design/rate-limiter/target/classes/models/Request.class`
- `04-System-Design/Low Level Design/rate-limiter/target/classes/utils/Timer.class`
- `04-System-Design/Low Level Design/rate-limiter/target/test-classes/RateLimitTest.class`
- `04-System-Design/Low Level Design/rate-limiter/target/test-classes/TestTimer.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/LoadBalancer.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/algorithms/ConsistentHashing.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/algorithms/Router.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/algorithms/WeightedRoundRobin.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/models/Node.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/models/Request.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/classes/models/Service.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/test-classes/LBTester.class`
- `04-System-Design/Low Level Design/service-orchestrator/target/test-classes/RouterTester.class`

</details>

