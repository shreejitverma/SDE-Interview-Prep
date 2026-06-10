# Django Internals: The ORM and Migration Engine


Django is the "batteries-included" web framework.
*   **The ORM**: Translates Python class definitions into SQL. It uses a complex tree-based query generator to handle joins and filters.
*   **Migrations**: Uses the `ast` module to analyze changes in models and generate the minimal SQL required to update the database schema.

---
