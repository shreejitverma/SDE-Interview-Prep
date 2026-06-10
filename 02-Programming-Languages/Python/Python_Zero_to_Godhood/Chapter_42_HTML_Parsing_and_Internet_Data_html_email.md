# HTML Parsing and Internet Data (`html`, `email`)


Interacting with web and mail systems requires robust handling of semi-structured and often non-compliant data.

### 53.1 `html.parser`: The SGML-Style Parser

The `html.parser` module is a structured markup parser that is more forgiving than XML parsers.
*   **Internals**: It uses a state-driven approach to identify tags and entities. It can handle "broken" HTML (e.g., missing closing tags) by following standardized tag-balancing rules.
*   **Security**: Always use `html.escape()` when outputting user data to prevent Cross-Site Scripting (XSS) vulnerabilities.

### 53.2 `email`: The Recursive Object Tree

The `email` package is a massive framework for managing email messages, which are fundamentally recursive structures (a message can contain a multipart message, which contains an attachment, etc.).

#### 1. The `Message` Object
An `email.message.EmailMessage` object consists of:
*   **Headers**: A dictionary-like mapping of field names to values.
*   **Payload**: Either a string (for simple text) or a list of `Message` objects (for multipart).

#### 2. Policy and Content Management
Modern Python (3.6+) introduced the **Policy** system.
*   **`policy.default`**: Uses the modern "Godhood" approachhandling Unicode, binary attachments, and folded headers automatically according to the latest RFCs (5322, 6532).
*   **Lazy Loading**: The `BytesParser` can lazily parse attachments, only reading them from the disk when the content is actually requested.

---


