# Using the Google Gen AI SDK
from google import genai

client = genai.Client(api_key="AIzaSyBHN13Wc8EQOFZBiGxW_DRLebNDSnNLW9Y")

# 1. Create a cache for the PDF (TTL defaults to 1 hour)
path_to_pdf = "/Users/shreejitverma/Documents/GitHub/SDE-Interview-Prep/02-Programming-Languages/C++/CPlusPlusNotesForProfessionals.pdf"
cache = client.caches.create(
    model='gemini-3-pro', # Or gemini-3-ultra-preview
    config={'ttl': '3600s'},
    files=[path_to_pdf]
)

# 2. Run your request using the cached content
response = client.models.generate_content(
    model='gemini-3-pro',
    prompt='''Read the first 100 pages of the pdf at '/Users/shreejitverma/Downloads/c-plus-plus-notes-for-professionals/c-plus-plus-notes-for-professionals_part1.pdf' and appropriately place
  the contents into the respective chapters in the **CPP_Zero_to_Godhood book, accordingly modify the chapter structure
  and content guidelines.** Implement content extraction, chapter mapping, strucpip install -U google-genaitural revision, and guideline updates.

  **Extraction:**
  - PDF text extraction.
  - Content segmentation.

  **Mapping:**
  - Topic identification.
  - Chapter assignment.

  **Revision:**
  - Chapter restructuring.
  - Content integration.
  - Guideline refinement.

  **Verification:**
  - Content accuracy check.
  - Structural integrity validation.
  **Implementation:**
  - Tool selection: PDF parsing library (e.g., PyMuPDF, pdfminer.six).
  - Extraction scope: First 100 pages, text only.
  - Segmentation strategy: Paragraph, section, or keyword-based.
  - Topic modeling: NLP techniques for theme detection.
  - Chapter mapping logic: Keyword matching, semantic similarity.
  - Structural changes: Add/merge/split chapters.
  - Content integration: Append, rephrase, or summarize.
  - Guideline updates: Update chapter scope, detail level.
  - Accuracy validation: Manual review, cross-referencing.
  - Integrity check: TOC consistency, flow.
  - Output format: Markdown.
  - Error handling: Unparseable pages, mapping conflicts.
  - Version control: Git commit for changes''',
    config={'cached_content': cache.name}
)