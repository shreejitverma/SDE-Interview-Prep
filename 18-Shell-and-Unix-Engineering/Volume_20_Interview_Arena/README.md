---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 20 - Interview Arena

Drills that turn the curriculum into interview performance, from senior to principal level.
Every drill has a model answer and the follow-up an interviewer is likely to ask.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)
- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)

## Chapters

1. [What Happens When You Type a Command and Press Enter](Chapter_01_What_Happens_When_You_Type_a_Command.md) - From the keyboard through the TTY, the parser, the expansions, fork, exec, and wait, to the exit status.
2. [Predict-the-Output Drills](Chapter_02_Predict_the_Output_Drills.md) - Quoting, expansion, subshell, and errexit puzzles with explanations.
3. [Debug-This-Script Drills](Chapter_03_Debug_This_Script_Drills.md) - Realistic broken scripts with root causes and fixes.
4. [One-Liner Fluency](Chapter_04_One_Liner_Fluency.md) - Common interview text-processing tasks and their model solutions.
5. [LeetCode Shell Problems](Chapter_05_LeetCode_Shell_Problems.md) - Model solutions for the four LeetCode shell problems, with complexity notes and portable alternatives.
6. [Design Drills](Chapter_06_Design_Drills.md) - A safe deploy script, log rotation, backups with integrity checks, and fleet-wide command fan-out.
7. [Unix Systems Interview Questions](Chapter_07_Unix_Systems_Interview_Questions.md) - Processes, signals, file descriptors, and terminals at senior to principal depth.
8. [The Staff Bar for Shell](Chapter_08_The_Staff_Bar_for_Shell.md) - What separates L5, L6, and L7+ answers on shell and Unix questions.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v20-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
