import re

SOURCE_FILE = "C++/Complete-CPP-Zero-to-Godhood.md"
NEW_CHAPTERS_DIR = "C++/CPP_Zero_to_Godhood"

NEW_C11_CHAPTERS = [
    "Chapter_22_C11_CORE_LANGUAGE_FEATURES.md",
    "Chapter_23_C11_SMART_POINTERS.md",
    "Chapter_24_C11_MOVE_SEMANTICS.md",
    "Chapter_25_C11_FUNCTIONAL_PROGRAMMING.md",
    "Chapter_26_C11_CONCURRENCY.md",
    "Chapter_27_C11_STANDARD_LIBRARY_ADDITIONS.md",
    "Chapter_28_C11_METAPROGRAMMING.md"
]

def read_file(path):
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()

def main():
    content = read_file(SOURCE_FILE)
    
    # 1. Identify where Chapter 7 (C++11) starts
    # Pattern: ## <a name="..."></a>CHAPTER 7: C++11 REVOLUTION
    start_match = re.search(r'##\s+<a name="[^"]+"></a>CHAPTER 7: C\+\+11 REVOLUTION', content)
    if not start_match:
        print("Could not find Chapter 7 start")
        return

    # 2. Identify where Chapter 9 (C++14) starts (End of C++11 block, skipping Ch 8 which we merge)
    end_match = re.search(r'##\s+<a name="[^"]+"></a>CHAPTER 9: C\+\+14 ENHANCEMENTS', content)
    if not end_match:
        print("Could not find Chapter 9 start")
        return

    start_idx = start_match.start()
    end_idx = end_match.start()

    # 3. Construct the new C++11 block
    new_c11_content = ""
    # Note: The new chapters I wrote use # Title. 
    # In the big file, chapters are ## Title. I should adjust indentation.
    # Also, I need to give them appropriate "CHAPTER X" headers for the parser.
    
    # We are replacing Source Ch 7 and 8.
    # The new chapters will logically be Source Ch 7, 8, 9, 10, 11, 12, 13.
    # This means Source Ch 9 (C++14) becomes Source Ch 14.
    # This renumbering in the source file is complex because of references.
    
    # SIMPLIFICATION:
    # Instead of renumbering the "Source Chapters" (which uses ## CHAPTER X), 
    # let's just insert the content as ## CHAPTER X: TITLE
    # and let the convert_to_latex.py script (which auto-numbers) handle the numbering.
    # The convert script effectively ignores the hardcoded number X in "CHAPTER X".
    
    current_fake_chap_num = 7
    for filename in NEW_C11_CHAPTERS:
        text = read_file(f"{NEW_CHAPTERS_DIR}/{filename}")
        # Extract title from first line "# TITLE"
        lines = text.split('\n')
        title_line = lines[0].replace('# ', '').strip()
        body = '\n'.join(lines[1:])
        
        header = f'## <a name="chapter-{current_fake_chap_num}-c11"></a>CHAPTER {current_fake_chap_num}: {title_line}\n'
        new_c11_content += header + body + "\n\n"
        current_fake_chap_num += 1

    # 4. Construct the final content
    # Pre-C++11 part
    pre_c11 = content[:start_idx]
    # Post-C++11 part (Starts at C++14)
    post_c11 = content[end_idx:]
    
    final_content = pre_c11 + new_c11_content + post_c11
    
    # 5. Overwrite
    with open(SOURCE_FILE, 'w', encoding='utf-8') as f:
        f.write(final_content)
    
    print("Successfully updated source file with expanded C++11 chapters.")

if __name__ == "__main__":
    main()
