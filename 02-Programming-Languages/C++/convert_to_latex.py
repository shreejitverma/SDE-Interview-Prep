import os
import re
import subprocess

SOURCE_FILE = "Complete-CPP-Zero-to-Godhood.md"
OUTPUT_DIR = "CPP_Zero_to_Godhood"

def sanitize_content(content):
    # Remove emojis and non-ASCII chars
    content = re.sub(r'[^\x00-\x7F]+', '', content)
    # Fix box drawing characters often found in tables
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
    
    def flush_chapter():
        nonlocal current_content, chapter_count
        if current_content:
            text = '\n'.join(current_content)
            # Determine prefix
            if current_title.startswith("Preface"):
                prefix = "00"
            elif current_title.startswith("Appendix"):
                match = re.search(r'Appendix\s+([A-Z])', current_title)
                prefix = f"Appendix_{match.group(1)}" if match else "Appendix_X"
            else:
                # This is a main chapter found by ## CHAPTER X
                # We use the X from CHAPTER X to be safe and consistent with index
                match = re.search(r'CHAPTER\s+(\d+)', current_title)
                if match:
                    prefix = f"Chapter_{match.group(1)}"
                else:
                    # Fallback for re-organized chapters if numbering is missing
                    chapter_count += 1
                    prefix = f"Chapter_{chapter_count}"
            
            clean_title = re.sub(r'[^\w\s-]', '', current_title).strip().replace(' ', '_')[:50]
            filename = f"{prefix}_{clean_title}"
            chapters.append((filename, current_title, text))
            current_content = []

    for line in lines:
        # STRICT MATCH for main chapters to avoid sub-headers being treated as chapters
        # Match: ## CHAPTER 1: TITLE or ## CHAPTER 1: TITLE (with anchor)
        chap_match = re.match(r'^##\s+(?:<a name="[^"]+"></a>)?CHAPTER\s+(\d+):\s*(.*)', line)
        if chap_match:
            flush_chapter()
            current_title = f"CHAPTER {chap_match.group(1)}: {chap_match.group(2).strip()}"
            continue

        # Match Appendix: # Appendix A: TITLE
        app_match = re.match(r'^#\s+Appendix\s+([A-Z]):\s*(.*)', line)
        if app_match:
            flush_chapter()
            current_title = f"Appendix {app_match.group(1)}: {app_match.group(2).strip()}"
            continue

        current_content.append(line)

    flush_chapter()
    return chapters

def convert_to_tex(filename, title, content):
    content = sanitize_content(content)
    md_path = os.path.join(OUTPUT_DIR, f"{filename}.md")
    with open(md_path, 'w', encoding='utf-8') as f:
        f.write(f"# {title}\n\n{content}")
    
    tex_path = os.path.join(OUTPUT_DIR, f"{filename}.tex")
    cmd = [
        "pandoc",
        md_path,
        "-f", "markdown",
        "-t", "latex",
        "-o", tex_path,
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
\usepackage{amsmath}
\usepackage{array}
\usepackage{calc}

% Pandoc compatibility
\newcommand{\passthrough}[1]{#1}
\providecommand{\tightlist}{%
  \setlength{\itemsep}{0pt}\setlength{\parskip}{0pt}}

% Page Layout
\geometry{margin=0.7in}
\setlength{\headheight}{15pt}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[LE,RO]{\thepage}
\fancyhead[RE]{\leftmark}
\fancyhead[LO]{\rightmark}
\raggedbottom

% Re-define chapter for compactness
\makeatletter
\def\@makechapterhead#1{%
  {\parindent \z@ \raggedright \normalfont
    \ifnum \c@secnumdepth >\m@ne
      \if@mainmatter
        \large\bfseries \@chapapp\space \thechapter
        \par\nobreak
        \vskip 5\p@
      \fi
    \fi
    \interlinepenalty\@M
    \Huge \bfseries #1\par\nobreak
    \vskip 20\p@
  }}
\makeatother

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

\title{
    \vspace*{2cm}
    \Huge \textbf{C++ Zero to Godhood} \\
    \vspace{0.5cm}
    \LARGE The Definitive Guide from C++98 to C++26 \\
    \vspace{2cm}
    \textbf{\Large Master the Beast.}
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
    
    for filename, title, _ in chapters:
        # Extract chapter number for volume insertion
        match = re.search(r'Chapter_(\d+)', filename)
        if match:
            chap_num = int(match.group(1))
            
            # Volume 01: C++98/03 (Chapters 1-9)
            if chap_num == 1 and current_volume < 1:
                master_content += r"\part{Volume 01: FOUNDATION (C++98/03)}" + "\n"
                current_volume = 1
            
            # Volume 02: C++11 (Chapters 10-15)
            elif chap_num == 10 and current_volume < 2:
                master_content += r"\part{Volume 02: MODERN REVOLUTION (C++11)}" + "\n"
                current_volume = 2
                
            # Volume 03: C++14 (Chapters 16-19)
            elif chap_num == 16 and current_volume < 3:
                master_content += r"\part{Volume 03: REFINEMENT (C++14)}" + "\n"
                current_volume = 3
                
            # Volume 04: C++17 (Chapters 20-25)
            elif chap_num == 20 and current_volume < 4:
                master_content += r"\part{Volume 04: MODERNIZATION (C++17)}" + "\n"
                current_volume = 4
                
            # Volume 05: C++20 (Chapters 26-31)
            elif chap_num == 26 and current_volume < 5:
                master_content += r"\part{Volume 05: GIGANTIC LEAP (C++20)}" + "\n"
                current_volume = 5
                
            # Volume 06: C++23 (Chapters 32-37)
            elif chap_num == 32 and current_volume < 6:
                master_content += r"\part{Volume 06: LATEST EVOLUTION (C++23)}" + "\n"
                current_volume = 6
                
            # Volume 07: C++26 (Chapter 38)
            elif chap_num == 38 and current_volume < 7:
                master_content += r"\part{Volume 07: THE NEXT FRONTIER (C++26)}" + "\n"
                current_volume = 7
                
            # Volume 08: Advanced Systems (Chapters 39-48)
            elif chap_num == 39 and current_volume < 8:
                master_content += r"\part{Volume 08: ADVANCED SYSTEMS}" + "\n"
                current_volume = 8

            # Volume 09: Specialized Domains (Chapters 49+)
            elif chap_num == 49 and current_volume < 9:
                master_content += r"\part{Volume 09: SPECIALIZED MASTERY}" + "\n"
                current_volume = 9

        if filename.startswith("Appendix") and current_volume < 10:
            master_content += r"\appendix" + "\n"
            current_volume = 10

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
    
    for filename, title, text in chapters:
        print(f"Converting {filename}...")
        convert_to_tex(filename, title, text)
        
    print("Creating master LaTeX file...")
    create_master_tex(chapters)
    print(f"Done. To compile: cd {OUTPUT_DIR} && pdflatex CPP_Zero_to_Godhood.tex")

if __name__ == "__main__":
    main()
