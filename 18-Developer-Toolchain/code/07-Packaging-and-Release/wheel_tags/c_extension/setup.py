from setuptools import Extension, setup

setup(
    name="c_abi3_example",
    version="0.1.0",
    ext_modules=[
        Extension(
            "c_abi3_example",
            sources=["c_abi3_example.c"],
            py_limited_api=True,
            define_macros=[("Py_LIMITED_API", "0x03080000")],
        )
    ],
    options={"bdist_wheel": {"py_limited_api": "cp38"}},
)
