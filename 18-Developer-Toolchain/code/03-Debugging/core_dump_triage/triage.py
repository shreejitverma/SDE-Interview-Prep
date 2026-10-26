import gdb
import os

frame = gdb.newest_frame()
func = frame.name()
line = frame.find_sal().line

count = 0
f = frame
frames_info = []
named_frames = 0
line_frames = 0
while f:
    f_sal = f.find_sal()
    f_name = f.name() or "??"
    f_file = os.path.basename(f_sal.symtab.filename) if f_sal.symtab else "??"
    f_line = f_sal.line if f_sal else 0
    frames_info.append(f"#{count} {f_name} at {f_file}:{f_line}")
    named_frames += f.name() is not None
    line_frames += bool(f_sal.symtab and f_sal.line)
    count += 1
    f = f.older()

try:
    signo = int(gdb.parse_and_eval("$_siginfo.si_signo"))
    sig_map = {11: "SIGSEGV", 6: "SIGABRT", 7: "SIGBUS", 4: "SIGILL", 8: "SIGFPE"}
    signame = sig_map.get(signo, f"SIGNAL {signo}")
except gdb.error:
    signame = "unknown (no siginfo in core)"

arch = frame.architecture()
pc = frame.pc()
block = gdb.block_for_pc(pc)
func_name = block.function.name if block and block.function else func
offset = pc - block.start if block else 0

pc_reg = "rip" if "x86-64" in arch.name() else "pc"
inst = arch.disassemble(pc)[0]["asm"]

mode = os.environ.get("TRIAGE_PASS", "unknown")
if mode == "without-debug-info":
    print(f"=== Pass: {mode} ===")
    print(f"Frames with a function name: {named_frames} of {count}")
    print(f"Frames with a source line: {line_frames} of {count}")
else:
    print(f"=== Pass: {mode} ===")
    print("Core source: " + os.environ.get("CORE_SOURCE", "unknown"))
    print("Build ID match: " + os.environ.get("BUILD_ID_EQUAL", "unknown"))

    print("\n=== Backtrace (Symbols Resolved via .build-id) ===")
    for line_info in frames_info:
        print(line_info)

    print("\n=== Register State ===")
    print(f"Faulting program counter ({pc_reg}): <{func_name}+{offset}>")

    print("\n=== Faulting Instruction ===")
    print(f"=> {inst}")

    print("\n=== Summary ===")
    print("Build ID equal: " + os.environ.get("BUILD_ID_EQUAL", "unknown"))
    print(f"Crashing function: {func}")
    print(f"Crashing line: {line}")
    print(f"Signal name: {signame}")
    print(f"Frame count: {count}")
