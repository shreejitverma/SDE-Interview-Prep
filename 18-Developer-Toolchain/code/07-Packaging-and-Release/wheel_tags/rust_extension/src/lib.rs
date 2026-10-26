use pyo3::prelude::*;

#[pyfunction]
fn rust_hello() -> PyResult<String> {
    Ok("Hello from Rust PyO3 ABI3 extension".to_string())
}

#[pymodule]
fn rust_abi3_example(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(rust_hello, m)?)?;
    Ok(())
}
