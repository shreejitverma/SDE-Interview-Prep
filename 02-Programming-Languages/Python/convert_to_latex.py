import os
import re
import subprocess

SOURCE_FILE = "Complete-Python-Zero-to-Godhood.md"
OUTPUT_DIR = "Python_Zero_to_Godhood"

def sanitize_content(content):
    # Remove emojis and non-ASCII chars
    content = re.sub(r'[^\x00-\x7F]+', '', content)
    # Fix box drawing characters often found in tables or diagrams
    content = content.replace('│', '|').replace('─', '-').replace('┌', '+').replace('┐', '+').replace('└', '+').replace('┘', '+').replace('├', '+').replace('┤', '+').replace('┬', '+').replace('┴', '+').replace('┼', '+')
    return content

def extract_chapters(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    lines = content.split('\n')
    
    chapters = []
    current_title = "Preface"
    current_content = []
    
    chapter_count = 0
    in_preface = True
    
    def flush_chapter():
        nonlocal current_content, chapter_count
        if current_content:
            text = '\n'.join(current_content)
            if current_title.startswith("Preface"):
                prefix = "00"
            elif current_title.startswith("Appendix"):
                match = re.search(r'Appendix\s+([A-Z])', current_title)
                prefix = f"Appendix_{match.group(1)}" if match else "Appendix_X"
            else:
                chapter_count += 1
                prefix = f"Chapter_{chapter_count:02d}"
            
            clean_title = re.sub(r'[^\w\s-]', '', current_title).strip().replace(' ', '_')[:50]
            filename = f"{prefix}_{clean_title}"
            chapters.append((filename, current_title, text))
            current_content = []

    for line in lines:
        # Match standard Chapter: ## CHAPTER X: TITLE
        chap_match = re.match(r'^##\s+CHAPTER\s+(\d+):\s*(.*)', line)
        if chap_match:
            flush_chapter()
            current_title = chap_match.group(2).strip()
            in_preface = False
            continue

        # Match Appendix: ## Appendix X: TITLE
        app_match = re.match(r'^##\s+Appendix\s+([A-Z]):\s*(.*)', line)
        if app_match:
            flush_chapter()
            current_title = f"Appendix {app_match.group(1)}: {app_match.group(2).strip()}"
            in_preface = False
            continue

        # Ignore volume headings from text body
        if line.startswith("# Volume") or line.startswith("# Phase"):
            continue 

        current_content.append(line)

    flush_chapter()
    return chapters

def convert_chapter(filename, title, content):
    md_filename = os.path.join(OUTPUT_DIR, f"{filename}.md")
    tex_filename = os.path.join(OUTPUT_DIR, f"{filename}.tex")

    content = sanitize_content(content)
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
\usepackage{amsmath}
\usepackage{fancyhdr}
\usepackage{calc}
\usepackage{array}

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
    pdftitle={Python Zero to Godhood},
    pdfauthor={Shreejit Verma},
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

% Pandoc compatibility commands
\newcommand{\passthrough}[1]{#1}
\providecommand{\tightlist}{%
  \setlength{\itemsep}{0pt}\setlength{\parskip}{0pt}}

\title{
    \vspace*{2cm}
    \Huge \textbf{Python Zero to Godhood} \\
    \vspace{0.5cm}
    \LARGE The Definitive Guide from Python 1.0 to Python 3.14 \\
    \vspace{2cm}
    \textbf{\Large Master the Runtime.}
}
\author{\Large Shreejit Verma}
\date{\today}

\begin{document}

\frontmatter
\maketitle
\tableofcontents

\mainmatter
"""
    current_volume = 0
    
    for filename, _, _ in chapters:
        # Extract chapter number
        match = re.search(r'Chapter_(\d+)_', filename)
        if match:
            chap_num = int(match.group(1))
            
            # Volume I: Classic Python & Core Engine (Chapters 1-6)
            if chap_num == 1 and current_volume < 1:
                master_content += r"\part{Volume I: Classic Python \& Core Engine}" + "\n"
                current_volume = 1
            
            # Volume II: The Python 3 Schism & Core Enhancements (Chapters 7-8)
            elif chap_num == 7 and current_volume < 2:
                master_content += r"\part{Volume II: The Python 3 Schism \& Core Enhancements}" + "\n"
                current_volume = 2
                
            # Volume III: Generators, Iterators, and Async Inception (Chapters 9-11)
            elif chap_num == 9 and current_volume < 3:
                master_content += r"\part{Volume III: Generators, Iterators, and Async Inception}" + "\n"
                current_volume = 3
                
            # Volume IV: Expressiveness & Developer Ergonomics (Chapters 12-13)
            elif chap_num == 12 and current_volume < 4:
                master_content += r"\part{Volume IV: Expressiveness \& Developer Ergonomics}" + "\n"
                current_volume = 4
                
            # Volume V: Structural Shifts & Pattern Matching (Chapters 14-16)
            elif chap_num == 14 and current_volume < 5:
                master_content += r"\part{Volume V: Structural Shifts \& Pattern Matching}" + "\n"
                current_volume = 5
                
            # Volume VI: Performance Leap & Runtime Mechanics (Chapters 17-19)
            elif chap_num == 17 and current_volume < 6:
                master_content += r"\part{Volume VI: Performance Leap \& Runtime Mechanics}" + "\n"
                current_volume = 6
                
            # Volume VII: The GIL-less Future & JIT Compilers (Chapters 20-22)
            elif chap_num == 20 and current_volume < 7:
                master_content += r"\part{Volume VII: The GIL-less Future \& JIT Compilers}" + "\n"
                current_volume = 7
                
            # Volume VIII: Runtime Internals & C Extensions (Chapters 23-25)
            elif chap_num == 23 and current_volume < 8:
                master_content += r"\part{Volume VIII: Runtime Internals \& C Extensions}" + "\n"
                current_volume = 8

            # Volume IX: High Performance & Low Latency Concurrency (Chapters 26+)
            elif chap_num == 26 and current_volume < 9:
                master_content += r"\part{Volume IX: High Performance \& Low Latency Concurrency}" + "\n"
                current_volume = 9

        # Handle preface
        if filename.startswith("00_"):
            master_content += f"\\input{{{filename}.tex}}\n"
        else:
            master_content += f"\\input{{{filename}.tex}}\n"

    master_content += r"\end{document}"

    with open(os.path.join(OUTPUT_DIR, "Python_Zero_to_Godhood.tex"), 'w', encoding='utf-8') as f:
        f.write(master_content)

def main():
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    print("Extracting chapters from markdown...")
    chapters = extract_chapters(SOURCE_FILE)
    print(f"Found {len(chapters)} chapters/sections.")
    
    processed_files = []
    for filename, title, content in chapters:
        print(f"Converting {filename} via Pandoc...")
        try:
            convert_chapter(filename, title, content)
            processed_files.append((filename, title, content))
        except Exception as e:
            print(f"Error converting {filename}: {e}")

    print("Creating master LaTeX file...")
    create_master_tex(processed_files)
    
    print("Compiling LaTeX to PDF...")
    try:
        # Run pdflatex twice to resolve table of contents links
        for _ in range(2):
            subprocess.run(["pdflatex", "-interaction=nonstopmode", "Python_Zero_to_Godhood.tex"], cwd=OUTPUT_DIR, check=True)
        print("Success! PDF built at Python_Zero_to_Godhood/Python_Zero_to_Godhood.pdf")
    except Exception as e:
        print(f"Error compiling PDF: {e}")

if __name__ == "__main__":
    main()
