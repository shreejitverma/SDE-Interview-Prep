---
type: moc
track: [ai-eng, sde, distinguished, quant-dev]
level:
status: draft
last_reviewed:
sources: []
---

# Python Packaging and Release Engineering

Python packaging has evolved from imperative setup scripts to standardized declarative metadata and hermetic binary wheels.
This section covers PEP 517, 518, 621, and 660 standards, the anatomy of wheels, manylinux baselines, the CPython abi3 Limited API, and free-threaded Python builds.
It explores multi-platform compilation pipelines with cibuildwheel, native build backends including scikit-build-core and maturin, rapid dependency resolution via uv and PEP 751 lockfiles, and application bundling.

```mermaid
flowchart TD
    HIST["Packaging Evolution<br/>(distutils to pyproject.toml)"] --> WHL["Python Wheels & abi3<br/>(manylinux_2_28, auditwheel)"]
    WHL --> FT["Free-Threaded Wheels<br/>(cp314t, WheelNext Variants)"]
    WHL --> CI["Cross-Platform Pipelines<br/>(cibuildwheel v4)"]
    HIST --> NB["Native Build Backends<br/>(scikit-build-core, maturin)"]
    HIST --> RES["Dependency Resolution<br/>(pip, uv, PEP 751 pylock.toml)"]
    RES --> CONDA["Polyglot Environments<br/>(Conda, Pixi Workspaces)"]
    HIST --> PUB["Secure Publishing<br/>(Trusted Publishing, PEP 740)"]
    HIST --> APP["Application Bundlers<br/>(zipapp, PEX, PyInstaller)"]
    HIST --> EP["Entry Points & Plugins<br/>(importlib, PEP 420)"]
```

## Learning order

1. [[Python-Packaging-History-and-pyproject-toml]]
2. [[Python-Wheels-Platform-Tags-Manylinux-and-ABI3]]
3. [[Python-Free-Threaded-Wheels-and-Wheel-Variants]]
4. [[Python-Cross-Platform-Wheel-Pipelines-with-cibuildwheel]]
5. [[Python-Native-Build-Backends-scikit-build-core-meson-python-and-maturin]]
6. [[Python-Dependency-Resolution-pip-uv-and-PEP-751-Lockfiles]]
7. [[Python-Conda-and-Pixi-for-Polyglot-Environments]]
8. [[Python-Trusted-Publishing-and-PEP-740-Attestations]]
9. [[Python-Shipping-Applications-Zipapp-PEX-and-Bundlers]]
10. [[Python-Entry-Points-Plugins-and-Namespace-Packages]]

Related books: [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_57_The_Python_Packaging_Ecosystem_PEP_517_to_Wheels.md|Python Zero to Godhood (Packaging Ecosystem)]] and [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/02-Python-Core.md|Python Backend Interview Prep (Core Dependencies)]].
