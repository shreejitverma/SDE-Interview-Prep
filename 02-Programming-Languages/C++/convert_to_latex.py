import os
import re
import subprocess

SOURCE_FILE = "C++/Complete-CPP-Zero-to-Godhood.md"
OUTPUT_DIR = "C++/CPP_Zero_to_Godhood"

# Map of old "Section" titles to promote to Chapters (appearing between Ch1 and Ch2)
PROMOTED_SECTIONS = [
    "ADVANCED POINTERS & MEMORY",
    "ADVANCED FUNCTIONS",
    "FUNCTION POINTERS & CALLBACKS",
    "ADVANCED ARRAYS",
    "ADVANCED STRINGS",
    "BITWISE OPERATIONS",
    "PREPROCESSOR DIRECTIVES",
    "TYPE CASTING",
    "ADVANCED CONTROL FLOW",
    "ENUMERATION & UNIONS",
    "CONST & VOLATILE",
    "INLINE FUNCTIONS & MACROS",
    "NAMESPACES",
    "FILE I/O ADVANCED",
    "ERROR HANDLING & DEBUGGING"
]

def sanitize_content(content):
    # Remove emojis and non-ASCII chars
    content = re.sub(r'[^\x00-\x7F]+', '', content)
    # Fix box drawing characters often found in tables
    content = content.replace('│', '|').replace('─', '-').replace('┌', '+').replace('┐', '+').replace('└', '+').replace('┘', '+').replace('├', '+').replace('┤', '+').replace('┬', '+').replace('┴', '+').replace('┼', '+')
    return content

def extract_chapters(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Regex for standard chapters: ## <a...>CHAPTER X: TITLE or ## CHAPTER X: TITLE
    # Regex for sections: # SECTION X: TITLE
    # Regex for Appendix: ## Appendix X: TITLE
    
    # We will iterate through the file line by line to maintain state
    lines = content.split('\n')
    
    chapters = []
    current_title = "Preface"
    current_content = []
    
    # State tracking
    chapter_count = 0
    in_preface = True
    
    # Helper to flush current chapter
    def flush_chapter():
        nonlocal current_content, chapter_count
        if current_content:
            text = '\n'.join(current_content)
            # Determine numbering
            if current_title.startswith("Preface"):
                prefix = "00"
            elif current_title.startswith("Appendix"):
                # Extract letter
                match = re.search(r'Appendix\s+([A-Z])', current_title)
                prefix = f"Appendix_{match.group(1)}" if match else "Appendix_X"
            else:
                chapter_count += 1
                prefix = f"Chapter_{chapter_count}"
            
            clean_title = re.sub(r'[^\w\s-]', '', current_title).strip().replace(' ', '_')[:50]
            filename = f"{prefix}_{clean_title}"
            chapters.append((filename, current_title, text))
            current_content = []

    for line in lines:
        # Check for Chapter Header
        # Match: ## <a name="..."></a>CHAPTER 1: TITLE or ## CHAPTER 1: TITLE
        # Note: The file has mixed ## and # for these.
        
        # 1. Standard Chapter
        chap_match = re.match(r'^##?\s+(?:<a name="[^"]+"></a>)?CHAPTER\s+\d+:\s*(.*)', line)
        if chap_match:
            flush_chapter()
            current_title = chap_match.group(1).strip()
            in_preface = False
            continue

        # 2. Promoted Section (Only if it's one of the specific legacy sections)
        sec_match = re.match(r'^#\s+SECTION\s+\d+:\s*(.*)', line)
        if sec_match:
            title = sec_match.group(1).strip()
            # Check if this is one of the "Lost Chapters" to be promoted
            is_promoted = False
            for p_title in PROMOTED_SECTIONS:
                if p_title in title:
                    is_promoted = True
                    break
            
            if is_promoted:
                flush_chapter()
                current_title = title
                in_preface = False
                continue
            else:
                # It's an internal section (e.g. in C++11 chapter). Convert to LaTeX section.
                line = f"## {title}" # Demote to subsection level in markdown (will become \section in latex)

        # 3. Appendix
        app_match = re.match(r'^##?\s+Appendix\s+([A-Z]):\s*(.*)', line)
        if app_match:
            flush_chapter()
            current_title = f"Appendix {app_match.group(1)}: {app_match.group(2).strip()}"
            in_preface = False
            continue

        # 4. Volume Headers (Ignore or keep as text?)
        # We'll ignore them to flatten the book, or maybe just add them as text.
        # User wants "No sections", implying flat structure.
        if line.startswith("# Volume"):
            continue 

        # Add line to current content
        current_content.append(line)

    # Flush last chapter
    flush_chapter()
    
    return chapters

def convert_chapter(filename, title, content):
    md_filename = os.path.join(OUTPUT_DIR, f"{filename}.md")
    tex_filename = os.path.join(OUTPUT_DIR, f"{filename}.tex")

    # Sanitize
    content = sanitize_content(content)
    
    # Ensure title is at the top if not present (Pandoc --top-level-division=chapter handles the file, but we need the heading text)
    # Actually, putting "# Title" at top of MD becomes \chapter{Title}
    final_content = f"# {title}\n\n{content}"

    with open(md_filename, 'w', encoding='utf-8') as f:
        f.write(final_content)

    cmd = [
        "pandoc",
        md_filename,
        "-o", tex_filename,
        "--from=markdown",
        "--to=latex",
        "--top-level-division=chapter",
        "--listings"
    ]
    subprocess.run(cmd, check=True)
    return filename

def create_master_tex(chapters):
    master_content = r"""\documentclass[11pt, a4paper, openany]{book}
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage{lmodern}
\usepackage{hyperref}
\usepackage{listings}
\usepackage{xcolor}
\usepackage{graphicx}
\usepackage{geometry}
\usepackage{longtable}
\usepackage{booktabs}
\usepackage{amssymb}
\usepackage{fancyhdr}

% Page Layout
\geometry{margin=1in}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[LE,RO]{\thepage}
\fancyhead[RE]{\leftmark}
\fancyhead[LO]{\rightmark}

% Colors for code
\definecolor{codegreen}{rgb}{0,0.6,0}
\definecolor{codegray}{rgb}{0.5,0.5,0.5}
\definecolor{codepurple}{rgb}{0.58,0,0.82}
\definecolor{backcolour}{rgb}{0.96,0.96,0.96}
\definecolor{techblue}{rgb}{0.0, 0.2, 0.6}

% Hyperlink styling
\hypersetup{
    colorlinks=true,
    linkcolor=techblue,
    filecolor=magenta,      
    urlcolor=cyan,
    pdftitle={C++ Zero to Godhood},
    pdfauthor={Community Guide},
}

% Code Listing Style
\lstdefinestyle{mystyle}{
    backgroundcolor=\color{backcolour},   
    commentstyle=\color{codegreen},
    keywordstyle=\color{magenta},
    numberstyle=\tiny\color{codegray},
    stringstyle=\color{codepurple},
    basicstyle=\ttfamily\footnotesize,
    breakatwhitespace=false,         
    breaklines=true,                 
    captionpos=b,                    
    keepspaces=true,                 
    numbers=left,                    
    numbersep=5pt,                  
    showspaces=false,                
    showstringspaces=false,
    showtabs=false,                  
    tabsize=2,
    frame=single,
    rulecolor=\color{codegray},
    literate={│}{|}1 {─}{-}1 {┌}{+}1 {┐}{+}1 {└}{+}1 {┘}{+}1 {├}{+}1 {┤}{+}1 {┬}{+}1 {┴}{+}1 {┼}{+}1
}
\lstset{style=mystyle}

% Fix for longtable width
\setlength{\LTleft}{0pt}
\setlength{\LTright}{0pt}

\title{
    \vspace*{2cm}
    \Huge \textbf{C++ Zero to Godhood} \\
    \vspace{0.5cm}
    \LARGE The Definitive Guide from C++98 to C++26 \\
    \vspace{2cm}
    \textbf{\Large Master the Beast.}
}
\author{\Large Community Edition}
\date{\today}

\begin{document}

\frontmatter
\maketitle
\tableofcontents

\mainmatter
"""
    # Logic to insert Volumes (Parts)
    current_volume = 0
    
    for filename, _, _ in chapters:
        # Extract chapter number
        match = re.search(r'Chapter_(\d+)_', filename)
        if match:
            chap_num = int(match.group(1))
            
            # Volume I: C++98/03 (Chapters 1-21)
            if chap_num == 1 and current_volume < 1:
                master_content += r"\part{Volume I: C++98/03 - The Foundation}" + "\n"
                current_volume = 1
            
            # Volume II: C++11 (Chapters 22-23)
            elif chap_num == 22 and current_volume < 2:
                master_content += r"\part{Volume II: C++11 - The Modern Revolution}" + "\n"
                current_volume = 2
                
            # Volume III: C++14 (Chapter 24)
            elif chap_num == 24 and current_volume < 3:
                master_content += r"\part{Volume III: C++14 - Refinement \& Generics}" + "\n"
                current_volume = 3
                
            # Volume IV: C++17 (Chapter 25)
            elif chap_num == 25 and current_volume < 4:
                master_content += r"\part{Volume IV: C++17 - Simplification \& Modernization}" + "\n"
                current_volume = 4
                
            # Volume V: C++20 (Chapter 26)
            elif chap_num == 26 and current_volume < 5:
                master_content += r"\part{Volume V: C++20 - The Gigantic Leap}" + "\n"
                current_volume = 5
                
            # Volume VI: C++23/26 (Chapters 27-28)
            elif chap_num == 27 and current_volume < 6:
                master_content += r"\part{Volume VI: C++23/26 - The Future}" + "\n"
                current_volume = 6
                
            # Volume VII: Advanced Systems (Chapters 29-43)
            elif chap_num == 29 and current_volume < 7:
                master_content += r"\part{Volume VII: Advanced Topics \& Systems Architecture}" + "\n"
                current_volume = 7
                
            # Volume VIII: Specialized Domains (Chapters 44+)
            elif chap_num == 44 and current_volume < 8:
                master_content += r"\part{Volume VIII: Specialized Domains \& Expert Mastery}" + "\n"
                current_volume = 8

        master_content += f"\\input{{{filename}.tex}}\n"

    master_content += r"\end{document}"

    with open(os.path.join(OUTPUT_DIR, "CPP_Zero_to_Godhood.tex"), 'w', encoding='utf-8') as f:
        f.write(master_content)

def main():
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    print("Extracting chapters...")
    chapters = extract_chapters(SOURCE_FILE)
    
    print(f"Found {len(chapters)} chapters/sections.")
    
    processed_files = []
    for filename, title, content in chapters:
        print(f"Converting {filename}...")
        try:
            convert_chapter(filename, title, content)
            processed_files.append((filename, title, content))
        except Exception as e:
            print(f"Error converting {filename}: {e}")

    print("Creating master LaTeX file...")
    create_master_tex(processed_files)
    
    print(f"Done. To compile: cd {OUTPUT_DIR} && pdflatex CPP_Zero_to_Godhood.tex")

if __name__ == "__main__":
    main()
