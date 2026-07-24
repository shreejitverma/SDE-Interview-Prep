# Volume 03 - Tool Use and the Agent Loop

This is the keystone volume of the track: by the end of it you have personally built a working agent from scratch against a real API, and you understand every line of the harness because you wrote it.
API shapes and vendor details are date-stamped as of early 2026; the loop, the ACI discipline, and the control-flow principles are stable.

## Chapters

- [Chapter 01 - What Is An Agent](Chapter_01_What_Is_An_Agent.md): competing definitions and their politics, the workflow-to-agent spectrum, the models-using-tools-in-a-loop framing, autonomy axes, and the four gates for deciding not to build an agent.
- [Chapter 02 - Function Calling Mechanics](Chapter_02_Function_Calling_Mechanics.md): how tool use works at the token level, JSON Schema contracts, tool_choice modes, parallel calls, and the complete Anthropic and OpenAI wire formats with real request and response JSON.
- [Chapter 03 - The Agent Loop From Scratch](Chapter_03_The_Agent_Loop_From_Scratch.md): a complete working agent in about 130 lines of Python against the Anthropic API, then iterated with streaming and a real tool surface of bash, file read, file write, and search.
- [Chapter 04 - Tool Design](Chapter_04_Tool_Design.md): the agent-computer interface, naming and description ergonomics, granularity trade-offs, errors as observations, token-efficient output with pagination and truncation, and lessons from SWE-agent and Claude Code.
- [Chapter 05 - Error Handling and Recovery](Chapter_05_Error_Handling_and_Recovery.md): a layered failure taxonomy, retry-with-feedback, validation layers, detectors for loops, surrender, and false success, budget guards, and graceful degradation.
- [Chapter 06 - Agentic Control Flow](Chapter_06_Agentic_Control_Flow.md): stop conditions, turn limits, interruption and steering, human-in-the-loop approval gates, checkpointing and resumability, pause and resume, streaming intermediate state, and the UX of agency.
- [Chapter 07 - Code Execution As A Tool](Chapter_07_Code_Execution_As_A_Tool.md): the REPL tool, the CodeAct pattern, the sandboxing spectrum from Docker through gVisor to Firecracker, cloud sandboxes, state persistence, and code mode as the answer to tool proliferation.
