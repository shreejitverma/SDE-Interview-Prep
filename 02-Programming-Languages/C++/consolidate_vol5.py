import os

base_dir = "C++/CPP_Zero_to_Godhood/Volume_05_Gigantic_Leap_C20"
tex_file = "C++/CPP_Zero_to_Godhood/CPP_Zero_to_Godhood.tex"

# List of old files to remove
old_files = [
    "Chapter_31_C20_REVOLUTIONARY_FEATURES"
]

# Clean up old files
for f in old_files:
    md_path = os.path.join(base_dir, f + ".md")
    tex_path = os.path.join(base_dir, f + ".tex")
    if os.path.exists(md_path):
        os.remove(md_path)
    if os.path.exists(tex_path):
        os.remove(tex_path)

# Update LaTeX file
new_chapters = [
    "Chapter_26_C20_Concepts",
    "Chapter_27_C20_Modules",
    "Chapter_28_C20_Coroutines",
    "Chapter_29_C20_Ranges",
    "Chapter_30_C20_Core_Language_Features",
    "Chapter_31_C20_Standard_Library_Additions"
]

if os.path.exists(tex_file):
    with open(tex_file, 'r') as f:
        lines = f.readlines()
    
    new_lines = []
    in_vol_5 = False
    vol_5_inserted = False
    
    for line in lines:
        if "\part{Volume V" in line:
            in_vol_5 = True
            new_lines.append(line)
            continue
            
        if "\part{Volume VI" in line:
            in_vol_5 = False
            if not vol_5_inserted:
                for ch in new_chapters:
                    new_lines.append(f"\input{{Volume_05_Gigantic_Leap_C20/{ch}}}\n")
                vol_5_inserted = True
            new_lines.append(line)
            continue
            
        if in_vol_5 and "\input{Volume_05_Gigantic_Leap_C20/" in line:
            continue # Skip old inputs
            
        new_lines.append(line)
        
    with open(tex_file, 'w') as f:
        f.writelines(new_lines)

print("Volume 5 consolidation complete.")