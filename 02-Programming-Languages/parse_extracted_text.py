import re
import os

def parse_extracted_text(input_file, output_dir):
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Create output dir
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        
    # Split content
    parts = re.split(r'(^\s*Chapter \d+: .+)', content, flags=re.MULTILINE)
    
    chapter_map = {}
    
    for i in range(1, len(parts), 2):
        header = parts[i].strip()
        body = parts[i+1]
        
        # Sanitize title
        # Remove dots and page numbers from header if present (common in TOC)
        # But for the filename, we just want the text.
        # "Chapter 1: Title ...... 2" -> "Chapter 1: Title"
        
        # distinct_header = re.sub(r'\s*\.+\s*\d+$', '', header) # Remove trailing dots and number
        # Actually, the regex split captures the whole line.
        
        safe_title = "".join([c if c.isalnum() else "_" for c in header])
        
        # Check if we already have this chapter
        if safe_title in chapter_map:
            # If current body is longer, replace it
            if len(body) > len(chapter_map[safe_title][1]):
                chapter_map[safe_title] = (header, body)
        else:
            chapter_map[safe_title] = (header, body)
            
    # Write to files
    for safe_title, (header, body) in chapter_map.items():
        filename = f"{safe_title}.txt"
        filepath = os.path.join(output_dir, filename)
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(header + "\n" + body)
            
        print(f"Saved {header}")

if __name__ == "__main__":
    parse_extracted_text("extracted_content.txt", "parsed_chapters")