import os

def read_chapter(filename):
    path = os.path.join("parsed_chapters", filename)
    if os.path.exists(path):
        with open(path, 'r', encoding='utf-8') as f:
            return f.read().strip()
    return ""

def integrate():
    target_file = "C++/Complete-CPP-Zero-to-Godhood.md"
    
    with open(target_file, 'r', encoding='utf-8') as f:
        lines = f.readlines()
        
    # Content for Chapter 1
    ch1_files = [
        "Chapter_1__Getting_started_with_C__.txt",
        "Chapter_2__Literals.txt",
        "Chapter_11__Loops.txt",
        "Chapter_15__Flow_Control.txt",
        "Chapter_117__Scopes.txt"
    ]
    
    ch1_content = """

---
### Professional Notes: Foundation & Basics

"""
    for fn in ch1_files:
        content = read_chapter(fn)
        if content:
            # Add a sub-header based on filename
            title = fn.replace("Chapter_", "").replace(".txt", "").replace("__", ": ").replace("_", " ")
            ch1_content += f"#### {title}\n\n{content}\n\n"

    # Content for Chapter 5
    ch5_files = [
        "Chapter_8__Arrays.txt",
        "Chapter_9__Iterators.txt",
        "Chapter_12__File_I_O.txt",
        "Chapter_13__C___Streams.txt"
    ]
    
    ch5_content = """

---
### Professional Notes: Standard Library & I/O

"""
    for fn in ch5_files:
        content = read_chapter(fn)
        if content:
             title = fn.replace("Chapter_", "").replace(".txt", "").replace("__", ": ").replace("_", " ")
             ch5_content += f"#### {title}\n\n{content}\n\n"

    # Find insertion points
    new_lines = []
    inserted_ch1 = False
    inserted_ch5 = False
    
    for line in lines:
        if "## <a name=\"chapter-2" in line and not inserted_ch1:
            new_lines.append(ch1_content)
            inserted_ch1 = True
            
        if "## <a name=\"chapter-6" in line and not inserted_ch5:
            new_lines.append(ch5_content)
            inserted_ch5 = True
            
        new_lines.append(line)
        
    with open(target_file, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)
        
    print("Integration complete.")

if __name__ == "__main__":
    integrate()