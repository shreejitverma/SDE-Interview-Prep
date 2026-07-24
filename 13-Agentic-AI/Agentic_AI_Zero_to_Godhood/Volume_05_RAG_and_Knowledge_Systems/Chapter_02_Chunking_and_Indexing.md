# Chapter 02 - Chunking and Indexing

## What you will master

- Why chunking exists at all, and why it is the highest-leverage, least-glamorous part of a RAG pipeline.
- The main chunking strategies - fixed, recursive, semantic, structural/AST-based, and layout-aware - with the trade-offs of each.
- How chunk size interacts with embedding quality, retrieval precision, and generation context.
- Metadata design and filtered retrieval, which most production systems depend on more than pure similarity.
- Contextual retrieval: prepending chunk context, including the approach Anthropic published in 2024.
- The unglamorous realities of document parsing pipelines, where most real-world RAG quality is won or lost.

## 1. Why chunking exists

Chunking is the decision of what unit of text gets one embedding and one retrieval slot.
It exists because of three separate constraints that are often conflated.

Embedding models have input limits and, more importantly, produce a single fixed-size vector regardless of input length.
A 512-token chunk and an 8,000-token chunk both become one vector of the same dimension, so the long chunk's vector is a blurrier average of more topics.
Empirically, embedding a whole heterogeneous document dilutes every individual fact's signal; a query about one paragraph must match a vector dominated by everything else.

Retrieval granularity determines what the generator sees.
Retrieve too small and the model gets a fact stripped of the context needed to interpret it.
Retrieve too large and you waste context tokens, push irrelevant text at the model, and reduce how many distinct sources fit in the prompt.

Attribution granularity determines what you can cite.
If your chunks are whole 40-page documents, your citations are useless; if they are single sentences, citations are precise but reconstructing the argument requires many of them.

A key mental model: the unit you embed and the unit you return to the model do not have to be the same.
Embed small for matching precision, then expand to the surrounding section, page, or parent chunk before generation.
This "small-to-big" (also called parent-document retrieval) pattern decouples the two decisions and resolves much of the size tension, at the cost of a lookup layer that maps child chunks to parents.

## 2. Chunking strategies

### 2.1 Fixed-size chunking

Split text every N tokens (or characters) with an overlap of M.
Typical starting points in practice are 256 to 1,024 tokens with 10-20 percent overlap.
Pros: trivial to implement, deterministic, uniform chunk statistics, no parser dependencies.
Cons: splits mid-sentence, mid-table, and mid-thought; the overlap duplicates content in the index and creates near-duplicate retrieval results; boundaries carry no meaning.
Fixed-size is the correct baseline: implement it first, measure, and only replace it when a failure analysis shows boundary-related misses.

### 2.2 Recursive character/token splitting

Try to split on the largest structural separator first (double newline for paragraphs), and only fall back to smaller separators (single newline, sentence end, whitespace) when a piece still exceeds the size limit.
This is the LangChain `RecursiveCharacterTextSplitter` idea and it is the pragmatic default in most codebases.
Pros: respects paragraph and sentence boundaries most of the time at near-zero implementation cost.
Cons: still blind to document semantics; a paragraph limit does not know that a heading belongs with the paragraphs under it, and lists or tables still get split awkwardly.

### 2.3 Semantic chunking

Embed sentences (or small windows) sequentially and place a chunk boundary where the embedding similarity between adjacent windows drops below a threshold, indicating a topic shift.
Pros: boundaries track topic changes rather than byte counts, producing chunks that are each "about one thing", which is exactly what a single embedding vector represents well.
Cons: it costs one embedding call per sentence at index time, thresholds need tuning per corpus, chunk sizes become highly variable, and evaluations across public benchmarks show inconsistent gains over recursive splitting - sometimes better, sometimes indistinguishable, occasionally worse.
Use it when failure analysis shows topic-mixture chunks are hurting you, not as a default.

### 2.4 Structural chunking, and AST-based chunking for code

When the document has explicit structure, use it.
For Markdown and HTML, split on the heading hierarchy and attach the heading path (for example "Deployment > Kubernetes > Secrets") to every chunk.
For code, character-based splitting is actively harmful: it slices functions in half and separates signatures from bodies.
AST-based chunking parses the file (tree-sitter is the standard tool because it covers many languages with one interface) and emits chunks aligned to syntactic units - functions, methods, classes - with oversize units split at nested boundaries.
Attach the enclosing scope (module, class name, function signature) as metadata or prefix text, because a method body without its class name is often unidentifiable.
Pros: chunks are semantically complete and compile-shaped, which matches how queries about code are phrased.
Cons: you need a parser per language, generated or minified code defeats it, and top-level script code without function structure still needs a fallback splitter.

### 2.5 Layout-aware chunking for PDFs and scans

PDF is a page-description format, not a text format; it stores positioned glyph runs with no reliable reading order, paragraph, or table markup.
Naive text extraction interleaves columns, shreds tables into meaningless rows of numbers, drops headers into body text, and loses figure captions.
Layout-aware parsing runs a document-layout model (or a vision-language model) to detect blocks - paragraphs, tables, figures, headers, footers - infer reading order, and reconstruct tables as Markdown or HTML.
As of early 2026 the practical options include open-source stacks (unstructured, Docling, marker, Nougat-style models) and commercial APIs (Azure Document Intelligence, AWS Textract, LlamaParse, and VLM-based parsing with frontier models).
Pros: this is the difference between a usable and a useless index for financial reports, scientific papers, and scanned contracts.
Cons: it is slow and costly relative to text extraction, layout models make their own errors (merged columns, hallucinated table cells with VLM parsers), and you must evaluate parser output as its own pipeline stage.
Rule of thumb: for PDF-heavy corpora, parsing quality dominates every downstream choice; a perfect retriever over garbled text loses to a mediocre retriever over clean text.

## 3. Chunk size trade-offs

There is no universally optimal chunk size; there is a set of pressures you balance per corpus and query type.

Pressure toward smaller chunks.
Embedding vectors represent short, single-topic text more faithfully, improving matching precision.
Smaller chunks mean finer-grained citations and less irrelevant text in the prompt.
Factoid queries ("what is the max retry count") match small chunks best.

Pressure toward larger chunks.
Facts need surrounding context to be interpretable; a lone table row or a pronoun-heavy sentence retrieved alone can mislead the generator.
Summarization-style and "explain this design" queries need whole sections.
Fewer, larger chunks reduce index size and per-query result assembly.
Modern long-context models remove the old hard constraint that forced tiny chunks, so the binding constraints are now embedding fidelity and prompt economy, not window size.

Practical guidance, stated with its evidence status: public chunking studies and vendor evaluations repeatedly land in the few-hundred-token range as a strong default for prose, but results vary by corpus and metric, so treat any specific number as a starting point for your own recall@k measurements rather than a truth.
The higher-leverage moves are usually not size tuning but: aligning boundaries with document structure, small-to-big expansion, and contextual enrichment (Section 5).

## 4. Metadata and filtered retrieval

Pure vector similarity over an undifferentiated chunk pool is rarely what production queries need.
Real queries carry implicit constraints: this tenant's documents only, current policy version only, source type equals contract, date within last quarter.
Metadata makes those constraints executable.

Attach to every chunk at index time: source document id and URI, title, section or heading path, author or owning team, document type, creation and last-modified timestamps, version or effective-date fields, language, access-control tags, and for code the file path and symbol name.
Then use metadata in three distinct ways.

Hard filtering: restrict the candidate set before or during vector search (tenant isolation and permissions must be hard filters, never similarity preferences).
Filtering interacts non-trivially with approximate indexes - Chapter 03 covers why pre-filtering can break HNSW graph connectivity and what filtered-search support to demand from a vector database.
Soft boosting: prefer recent documents or authoritative sources by score adjustment at ranking time.
Attribution and display: citations, dedup by document, and grouping results by source.

Two design warnings.
First, access control through metadata filters is a security boundary; test it adversarially, because a missed filter leaks another tenant's data directly into a prompt.
Second, plan metadata before indexing: backfilling a new metadata field means re-processing the corpus, and while some stores allow metadata updates in place, your pipeline must support re-indexing regardless, because embedding model upgrades force full re-embeds anyway.

## 5. Contextual retrieval: giving chunks back their context

A chunk ripped from its document loses information that lives outside its boundaries: which company the pronoun "it" refers to, which product version the section describes, which fiscal year the table covers.
This causes retrieval misses (the query says "Acme Q2 revenue", the chunk says "revenue grew 3 percent" with "Acme" only in the document title) and generation errors (the model misattributes the fact).

The general fix is to enrich each chunk with context before embedding it.
Cheap static versions: prepend the document title and the heading path to every chunk's text; for code, prepend file path and enclosing symbol.
This costs almost nothing and should be considered table stakes.

The LLM-generated version is what Anthropic published as "Contextual Retrieval" in September 2024.
For each chunk, an LLM is given the whole document plus the chunk and asked to write a short blurb (typically 50-100 tokens) situating the chunk within the document; the blurb is prepended to the chunk before embedding, and before BM25 indexing in the hybrid variant.
Their published evaluation reported that contextual embeddings reduced top-20-chunk retrieval failure rate substantially, with further reduction when combined with contextual BM25 and reranking - consult the original post for the exact figures rather than quoting them from memory.
The economics work because of prompt caching: the full document is cached across the many per-chunk calls, so the marginal cost per chunk is the blurb generation, making one-time indexing cost modest relative to the retrieval quality gain.
Trade-offs: index-time cost and latency scale with corpus size, the blurbs are LLM output and can be subtly wrong, and re-indexing cost rises accordingly.
Contextual retrieval competes with small-to-big expansion: both re-attach context, one at embed time (helping matching) and one at read time (helping generation); they compose, and matching-side context is the one that fixes retrieval misses.

## 6. Indexing pipeline realities

The textbook pipeline is: load, parse, chunk, embed, upsert.
The production pipeline is a data-engineering system with all the usual failure modes, plus a few of its own.

Ingestion and change detection.
Sources are heterogeneous (wikis, drives, ticket systems, repos) with different APIs, auth, and rate limits.
You need incremental sync: detect created, updated, and deleted documents, and propagate deletions to the index - stale chunks from deleted documents are a common and embarrassing failure ("the bot cited a policy we retracted last year").
Content hashing per document and per chunk prevents redundant re-embedding of unchanged text.

Parsing failure handling.
A corpus of any size contains corrupt files, password-protected PDFs, 400 MB slide decks, images of text, and files whose extension lies about their format.
Parse in isolated workers with timeouts, quarantine failures with reasons, and report coverage: "94 percent of documents indexed" is a number someone must own.

Consistency and versioning.
Embedding model upgrades change the vector space; vectors from different models are not comparable, so an upgrade means re-embedding everything and usually maintaining two indexes during cutover.
Chunker changes similarly invalidate the index.
Version your pipeline configuration (parser version, chunker parameters, embedding model id) and stamp it on every chunk so you can audit what produced any given index entry.

Throughput and cost.
Index-time work is batch work: batch embedding calls, parallel parsing, and backpressure.
For large corpora, embedding cost and time are material; measure tokens per document and budget before promising a full re-index turnaround.

Quality measurement at each stage.
Sample parsed output and eyeball it against source documents on a schedule; parsing regressions are silent otherwise.
Track chunk statistics (size distribution, empty or near-empty chunks, duplicate rates) as pipeline health metrics.
The theme of this section is blunt: most RAG systems that perform badly in production are not failing at vector search; they are failing at parsing, chunk hygiene, or stale-index management, and no retriever fixes upstream garbage.

## 7. A minimal, honest reference implementation

The following sketch shows the load-parse-chunk-enrich-embed flow with the decisions from this chapter made explicit.
It uses recursive splitting with structural prefixes, which is the pragmatic default; API calls are represented abstractly to stay provider-neutral.

```python
from dataclasses import dataclass, field
import hashlib

@dataclass
class Chunk:
    doc_id: str
    text: str            # what gets embedded (prefix + body)
    body: str            # what gets shown/cited
    metadata: dict = field(default_factory=dict)

def split_recursive(text: str, max_tokens: int, seps=("\n\n", "\n", ". ")) -> list[str]:
    if count_tokens(text) <= max_tokens:
        return [text]
    for sep in seps:
        parts = [p for p in text.split(sep) if p.strip()]
        if len(parts) > 1:
            merged, buf = [], ""
            for p in parts:
                cand = (buf + sep + p) if buf else p
                if count_tokens(cand) > max_tokens and buf:
                    merged.append(buf)
                    buf = p
                else:
                    buf = cand
            if buf:
                merged.append(buf)
            return [c for m in merged for c in split_recursive(m, max_tokens, seps)]
    return hard_split_by_tokens(text, max_tokens)  # fallback: no separator worked

def build_chunks(doc) -> list[Chunk]:
    chunks = []
    for section in doc.sections:                      # from a structure-aware parser
        prefix = f"{doc.title} > {' > '.join(section.heading_path)}\n"
        for body in split_recursive(section.text, max_tokens=512):
            chunks.append(Chunk(
                doc_id=doc.id,
                text=prefix + body,                    # structural context for matching
                body=body,
                metadata={
                    "title": doc.title,
                    "heading_path": section.heading_path,
                    "source_uri": doc.uri,
                    "modified_at": doc.modified_at,
                    "acl_tags": doc.acl_tags,
                    "pipeline_version": PIPELINE_VERSION,
                    "content_hash": hashlib.sha256(body.encode()).hexdigest(),
                },
            ))
    return chunks

def index_document(doc, store, embedder):
    chunks = build_chunks(doc)
    new = [c for c in chunks if not store.has_hash(c.doc_id, c.metadata["content_hash"])]
    vectors = embedder.embed_batch([c.text for c in new])   # batch, not per-chunk calls
    store.delete_stale(doc.id, keep_hashes={c.metadata["content_hash"] for c in chunks})
    store.upsert(new, vectors)
```

The load-bearing details: the embedded text differs from the cited body, hashes gate re-embedding, stale chunks are deleted on every re-index of a document, and the pipeline version is stamped on each chunk.
Swapping `split_recursive` for an AST chunker (code) or a layout-aware parser (PDFs) changes only `build_chunks`.

## Exercises

1. Take one real Markdown document and one real PDF from a corpus you care about.
   Chunk both with fixed-size (512 tokens, 15 percent overlap) and with structure-aware splitting, and manually inspect ten chunks from each.
   Count how many chunks are uninterpretable without their neighbors.
2. Implement small-to-big retrieval: embed 256-token child chunks, retrieve top-5, and return their parent sections to the generator.
   Compare answer quality against returning the child chunks directly on ten questions.
3. Run naive text extraction and a layout-aware parser on a PDF containing at least one table.
   Ask the same numeric question against both versions and document the failure mode of the naive one.
4. Implement contextual enrichment two ways: (a) static title-plus-heading prefixes, (b) LLM-generated situating blurbs per Anthropic's contextual retrieval recipe.
   Measure top-10 retrieval hit rate on 20 queries against a plain-chunk baseline, and compute the index-time cost per 1,000 chunks for (b).
5. Build change detection: hash chunks, re-run the pipeline on a modified document, and verify that only changed chunks are re-embedded and that deleted sections disappear from the index.
6. For a code repository, chunk one large file with a recursive character splitter and with tree-sitter at function granularity.
   Query for a specific function's behavior and compare what each index retrieves.

## Godhood check

You have mastered this chapter when you can do the following without notes.

- Explain why one embedding per long heterogeneous chunk degrades matching, and why embed-unit and return-unit can differ.
- Describe five chunking strategies and name the corpus type and failure mode that motivates each.
- Argue the small-chunk versus large-chunk trade-off in terms of embedding fidelity, interpretability, citation granularity, and prompt economy.
- Design a metadata schema for a multi-tenant document corpus and state which fields are security boundaries versus ranking hints.
- Explain contextual retrieval, why prompt caching makes it affordable, and how it differs from and composes with parent-document expansion.
- List the top three reasons production RAG indexes silently rot, and the pipeline mechanism that prevents each.
