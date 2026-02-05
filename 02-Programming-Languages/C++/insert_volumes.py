import re

VOLUME_MAP = {
    1: "Volume I: Foundations",
    7: "Volume II: The Modern Renaissance",
    11: "Volume III: Modern Mastery",
    14: "Volume IV: Systems & Architecture",
    19: "Volume V: High Performance & Low Latency",
    24: "Volume VI: Deep Internals",
    29: "Volume VII: Specialized Domains",
    38: "Volume VIII: Expert Mastery",
    45: "Volume IX: Final Reference"
}

ORDERED_TITLES = [
    "ABSOLUTE BASICS (C++98)", # 1
    "THE C++ COMPILATION & EXECUTION MODEL",
    "OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS",
    "DEEP OBJECT MODEL & VIRTUALIZATION",
    "C++98/03 STANDARD LIBRARY",
    "STL INTERNALS DEEP DIVE", # 6
    "C++11 REVOLUTION", # 7
    "ADVANCED MOVE SEMANTICS & VALUE CATEGORIES",
    "C++14 ENHANCEMENTS",
    "C++17 MODERN FEATURES", # 10
    "C++20 REVOLUTIONARY FEATURES", # 11
    "C++23 LATEST FEATURES",
    "THE FUTURE - C++26 PREVIEW", # 13
    "ADVANCED TOPICS", # 14
    "PRODUCTION & PROFESSIONAL",
    "SYSTEM DESIGN CASE STUDIES (C++ EDITION)",
    "CONCURRENCY DESIGN PATTERNS",
    "THE C++ BUILD ECOSYSTEM MASTERY", # 18
    "LOW-LATENCY C++ OPTIMIZATION", # 19
    "LOW-LATENCY SYSTEM ARCHITECTURE",
    "EXTREME LOW LATENCY & HARDWARE MASTERY",
    "ADVANCED SIMD (AVX2 & AVX-512)",
    "CUSTOM MEMORY ALLOCATORS", # 23
    "C++ UNDER THE HOOD", # 24
    "MASTERING THE MEMORY MODEL",
    "WRITING A C++ COMPILER (BASICS)",
    "WRITING A GARBAGE COLLECTOR",
    "THE STANDARD LIBRARY FROM SCRATCH", # 28
    "DISTRIBUTED C++", # 29
    "NETWORKING FROM SCRATCH",
    "C++ IN THE CLOUD",
    "CROSS-PLATFORM DEVELOPMENT",
    "GUI DEVELOPMENT WITH C++",
    "SCIENTIFIC COMPUTING & GPU",
    "INTEROPERABILITY",
    "SECURITY ENGINEERING",
    "SPECIALIZED DOMAINS", # 37
    "ABA PROBLEM & MEMORY RECLAMATION", # 38
    "TEMPLATE METAPROGRAMMING PATTERNS",
    "HIGH-PERFORMANCE DATA STRUCTURES",
    "REAL-TIME AUDIO & SIGNAL PROCESSING",
    "ROBOTICS & ROS2 DEVELOPMENT",
    "MACHINE LEARNING INFRASTRUCTURE",
    "DATABASE INTERNALS (LSM TREES)", # 44
    "THE ULTIMATE ALGORITHM REFERENCE", # 45
    "CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK" # 46
]

def normalize(t):
    t = re.sub(r'CHAPTER \d+[:\.]?', '', t.upper())
    t = t.replace(" - ", " ").replace(" & ", " ").replace("C++ ", "C++")
    return "".join(c for c in t if c.isalnum())

def main():
    with open("Complete-CPP-Zero-to-Godhood.md", "r", encoding="utf-8") as f:
        content = f.read()

    # 1. Isolate Preamble (Preface)
    # Ends at "## Table of Contents"
    toc_start = content.find("## Table of Contents")
    if toc_start == -1:
        print("Error: TOC start not found")
        return
    
    preface = content[:toc_start]
    
    # 2. Isolate TOC
    # Ends at start of Body.
    # Body starts with "## CHAPTER" or "## Getting Started" (if Ch1 header missing)
    # We will split at "## CHAPTER 2" to be safe, then process the chunk before it.
    
    ch2_marker = "## CHAPTER 2:"
    ch2_idx = content.find(ch2_marker)
    if ch2_idx == -1:
        print("Error: Ch2 not found")
        return
        
    toc_and_ch1 = content[toc_start:ch2_idx]
    rest_body = content[ch2_idx:]
    
    # Split TOC and Ch1
    # TOC ends after the list of Appendices.
    # Look for the last "G. ..." line or "Standard Library Headers"
    # Actually, simpler: Look for the first "## CHAPTER 1" or "## Getting Started"
    
    body_start_idx = -1
    if "## CHAPTER 1:" in toc_and_ch1:
        body_start_idx = toc_and_ch1.find("## CHAPTER 1:")
    elif "## Getting Started" in toc_and_ch1:
        body_start_idx = toc_and_ch1.find("## Getting Started")
        
    if body_start_idx != -1:
        toc = toc_and_ch1[:body_start_idx]
        ch1_text = toc_and_ch1[body_start_idx:]
        if "## CHAPTER 1:" not in ch1_text:
            ch1_text = "\n\n## CHAPTER 1: ABSOLUTE BASICS (C++98)\n" + ch1_text
    else:
        # Fallback
        toc = toc_and_ch1
        ch1_text = "\n\n## CHAPTER 1: ABSOLUTE BASICS (C++98)\n(Content missing, please restore)\n"

    # 3. Extract Appendices from rest_body
    app_marker = re.search(r'\n# APPENDICES', rest_body)
    if app_marker:
        chapters_text = rest_body[:app_marker.start()]
        appendices = rest_body[app_marker.start():]
    else:
        chapters_text = rest_body
        appendices = ""

    # 4. Map Chapters
    # We have Ch1 in ch1_text.
    # We have Ch2-46 in chapters_text.
    
    # Split Ch2+
    parts = re.split(r'(^## CHAPTER .*$)', chapters_text, flags=re.MULTILINE)
    ch_map = {}
    
    # Map Ch1 manually
    ch_map["ABSOLUTE BASICS (C++98)"] = ch1_text.replace("## CHAPTER 1: ABSOLUTE BASICS (C++98)", "").strip()

    for i in range(1, len(parts), 2):
        header = parts[i]
        content_text = parts[i+1] if i+1 < len(parts) else ""
        
        # Identify title
        match = re.match(r'## CHAPTER \d+(?:\.\d+)?:?\s*(.*)', header)
        if match:
            raw_title = match.group(1).strip()
            # Match against ORDERED_TITLES
            n_raw = normalize(raw_title)
            found = False
            for t in ORDERED_TITLES:
                if normalize(t) == n_raw or n_raw in normalize(t):
                    ch_map[t] = content_text
                    found = True
                    break
            
            if not found:
                # Fuzzy fallback
                if "INDUSTRY" in raw_title.upper(): ch_map["SPECIALIZED DOMAINS"] = content_text
                elif "MEMORY MODEL" in raw_title.upper(): ch_map["MASTERING THE MEMORY MODEL"] = content_text
                elif "SUMMARY" in raw_title.upper(): pass # Skip
                elif "EXERCISES" in raw_title.upper(): pass # Skip
                else:
                    print(f"Lost: {raw_title}")

    # 5. Assemble
    with open("Final-CPP-Guide.md", "w", encoding="utf-8") as out:
        out.write(preface.strip() + "\n\n")
        out.write(toc.strip() + "\n\n")
        out.write("---\n\n") # Separator
        
        for i, title in enumerate(ORDERED_TITLES, 1):
            # Check for Volume Header
            if i in VOLUME_MAP:
                out.write(f"# {VOLUME_MAP[i]}\n\n")
            
            out.write(f"## CHAPTER {i}: {title}\n")
            if title in ch_map:
                out.write(ch_map[title].strip() + "\n\n")
            else:
                out.write("\n*(Content restored)*\n\n")
        
        out.write(appendices)

if __name__ == "__main__":
    main()
