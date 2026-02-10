from pdfminer.high_level import extract_text
from pdfminer.layout import LAParams
import sys

def extract_first_n_pages(pdf_path, output_path, n=100):
    print(f"Extracting first {n} pages from {pdf_path}...")
    try:
        # maxpages=n (0-indexed effectively for count, but page_numbers is None implies start from beginning)
        # page_numbers are 0-indexed list.
        # extract_text allows 'page_numbers' or 'maxpages'.
        # However, extract_text definition: extract_text(pdf_file, password='', page_numbers=None, maxpages=0, caching=True, codec='utf-8', laparams=None)
        # If maxpages=0 (default), read all.
        
        text = extract_text(pdf_path, maxpages=n, laparams=LAParams())
        
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(text)
        print(f"Extraction complete. Saved to {output_path}")
        
    except Exception as e:
        print(f"Error extracting text: {e}")
        sys.exit(1)

if __name__ == "__main__":
    pdf_path = "C++/CPlusPlusNotesForProfessionals.pdf"
    output_path = "extracted_content.txt"
    extract_first_n_pages(pdf_path, output_path, n=100)
