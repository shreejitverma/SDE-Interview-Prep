import os
import subprocess

BASE_DIR = "C++/CPP_Zero_to_Godhood"

def convert_md_to_tex(root_dir):
    for dirpath, dirnames, filenames in os.walk(root_dir):
        # Sort directories to ensure consistent order (Volumes)
        dirnames.sort()
        
        for filename in filenames:
            if filename.endswith(".md"):
                md_path = os.path.join(dirpath, filename)
                tex_path = os.path.join(dirpath, filename.replace(".md", ".tex"))
                
                print(f"Converting {md_path} to LaTeX...")
                
                # Check if tex file is older than md file
                if os.path.exists(tex_path) and os.path.getmtime(tex_path) > os.path.getmtime(md_path):
                    print(f"Skipping {tex_path} (up to date)")
                    continue

                cmd = [
                    "pandoc",
                    md_path,
                    "-o", tex_path,
                    "--from=markdown",
                    "--to=latex",
                    "--top-level-division=chapter",
                    "--listings"
                ]
                try:
                    subprocess.run(cmd, check=True)
                except subprocess.CalledProcessError as e:
                    print(f"Error converting {md_path}: {e}")

if __name__ == "__main__":
    convert_md_to_tex(BASE_DIR)
