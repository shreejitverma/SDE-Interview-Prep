# Vault Audit

Generated 2026-09-21 by `python3 tools/audit_vault.py` over git-tracked files.
Re-run the script after every structural change; this file is its output and should not be hand-edited.
0 files in private locations (`tools/private_paths.py`) are excluded; `tools/audit_pii.py` inventories them into a private path.

## Summary

| Check | Result |
| :--- | ---: |
| Tracked files | 15145 |
| Markdown notes | 1517 |
| Internal links checked | 9317 |
| Broken links (links into private locations are not counted) | 0 |
| Wikilink aliases that split a table cell | 0 |
| Broken wikilinks fixable by unique basename | 0 |
| Orphan knowledge notes (no inbound links) | 3 |
| Archived drafts (`_archive/`, `_consolidated*/`) | 305 |
| Knowledge notes without frontmatter | 0 |
| Note folders without README (depth <= 3) | 0 |
| Notes with emojis / total emojis | 0 / 0 |
| Notes with em dashes / total em dashes | 0 / 0 |
| Identical-content groups / redundant MB | 305 / 41.9 |
| Vendored or imported repos | 10 |
| Tracked build junk | 0 |
| Files >= 5 MB / total MB | 1 / 5.1 |

## 1. File counts by top-level folder

| Folder | Files | Notes | Code | Papers | Other | MB | Last touched |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `(root)` | 13 | 8 | 0 | 0 | 5 | 0.1 | 2026-09-21 |
| `.github` | 1 | 0 | 0 | 0 | 1 | 0.0 | 2026-09-21 |
| `.obsidian` | 43 | 0 | 0 | 0 | 43 | 0.0 | 2026-09-21 |
| `00-Start-Here` | 3 | 3 | 0 | 0 | 0 | 0.0 | 2026-09-21 |
| `01-CS-Foundations` | 151 | 65 | 59 | 0 | 27 | 1.5 | 2026-09-21 |
| `02-Programming-Languages` | 5819 | 798 | 2856 | 466 | 1699 | 112.0 | 2026-09-21 |
| `03-Data-Structures-Algorithms` | 7708 | 82 | 7506 | 0 | 120 | 15.2 | 2026-09-21 |
| `04-System-Design` | 880 | 66 | 657 | 0 | 157 | 7.5 | 2026-09-21 |
| `05-Quantitative-Finance` | 8 | 1 | 7 | 0 | 0 | 0.0 | 2026-09-21 |
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
| `tools` | 15 | 1 | 12 | 0 | 2 | 0.1 | 2026-09-21 |

## 2. Duplicate and overlapping sections

### Folders whose names normalize to the same topic

- **design pattern**: `04-System-Design/03-Design-Patterns`, `04-System-Design/Design Patterns`, `04-System-Design/design-patterns-java`, `04-System-Design/design-patterns-python`
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
| `01-CS-Foundations/Object-Oriented-Programming` | `04-System-Design/design-questions` | 1 |

### Largest identical-content groups (redundant bytes)

| First copy | Copies | Redundant MB |
| :--- | ---: | ---: |
| `02-Programming-Languages/Python/python in depth/Django_Blog/09-Update-User-Profile/django_project/media/profile_pics/large.jpg` | 14 | 35.3 |
| `02-Programming-Languages/Python/python in depth/Django_Blog/07-Login-Logout-Authentication/django_project/media/profile_pics/pic.jpg` | 9 | 2.4 |
| `02-Programming-Languages/JavaScript/All in One/dom-tutorial/bg-image.jpg` | 2 | 1.6 |
| `02-Programming-Languages/Python/python in depth/Python/Flask_Blog/07-User-Account-Profile-Pic/flaskblog/static/profile_pics/85ed1b444539873d.png` | 7 | 0.5 |
| `02-Programming-Languages/C++/Coding/03.FirstSteps/3.2FirstCppProgram/.gitignore` | 461 | 0.4 |
| `02-Programming-Languages/Python/python in depth/Django_Blog/09-Update-User-Profile/django_project/media/profile_pics/large_rbSbk8j.jpg` | 7 | 0.2 |
| `02-Programming-Languages/Python/python in depth/Django_Blog/07-Login-Logout-Authentication/django_project/media/default.jpg` | 16 | 0.2 |
| `02-Programming-Languages/Python/python in depth/Django_Blog/07-Login-Logout-Authentication/django_project/db.sqlite3` | 2 | 0.1 |
| `02-Programming-Languages/C++/Coding/03.FirstSteps/3.2FirstCppProgram/CMakeLists.txt` | 321 | 0.1 |
| `02-Programming-Languages/Python/python in depth/Python/Flask_Blog/07-User-Account-Profile-Pic/flaskblog/static/profile_pics/b6e1c53325f88b74.png` | 10 | 0.1 |
| `02-Programming-Languages/Python/python in depth/Python/Flask_Blog/11-Blueprints/flaskblog/site.db` | 3 | 0.1 |
| `02-Programming-Languages/Python/python in depth/Django_Blog/11-Pagination/django_project/posts.json` | 5 | 0.1 |
| `02-Programming-Languages/Python/python in depth/Python/Flask_Blog/07-User-Account-Profile-Pic/flaskblog/static/profile_pics/7798432669b8b3ac.jpg` | 10 | 0.1 |
| `02-Programming-Languages/C++/Coding/42.FunctionLikeEntities/42.10LambdaFunctionsAsCallbacks/boxcontainer.h` | 7 | 0.0 |
| `02-Programming-Languages/Python/python in depth/Python/Flask_Blog/03-Forms-and-Validation/templates/register.html` | 8 | 0.0 |

### Folder pairs with overlapping note or code names (topical overlap)

| Folder A | Folder B | Shared names | Overlap of smaller |
| :--- | ---: | ---: | ---: |
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
| `02-Programming-Languages/Python/python in depth` | 2045 | 2026-09-21 |
| `04-System-Design/design-patterns-java` | 177 | 2026-09-21 |
| `04-System-Design/design-questions` | 172 | 2026-09-21 |
| `04-System-Design/Design Patterns/python-patterns` | 106 | 2026-09-21 |
| `03-Data-Structures-Algorithms/01-Topics/General-DSA` | 94 | 2026-09-21 |
| `04-System-Design/Low Level Design` | 58 | 2026-09-21 |
| `01-CS-Foundations/Operating-Systems/code/os` | 17 | 2026-07-24 |
| `01-CS-Foundations/Object-Oriented-Programming/code/oop` | 16 | 2026-07-24 |
| `04-System-Design/InterviewReady/splitwise` | 14 | 2026-07-24 |

### Large files

Files of 5 MB or more; candidates for Git LFS, external links, or removal.

| File | MB |
| :--- | ---: |
| `02-Programming-Languages/C++/CPlusPlusNotesForProfessionals.pdf` | 5.1 |

### Tracked build junk

`.DS_Store`, CMake build trees, object files, and similar that should be gitignored.

| Folder | Files |
| :--- | ---: |

<details>
<summary>All 0 junk files</summary>


</details>

