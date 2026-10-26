#define Py_LIMITED_API 0x03080000
#include <Python.h>

static PyObject *c_hello(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyUnicode_FromString("Hello from C Limited API extension");
}

static PyMethodDef Methods[] = {
    {"c_hello", c_hello, METH_NOARGS, "Return greeting string."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "c_abi3_example",
    "Minimal C ABI3 extension module",
    -1,
    Methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_c_abi3_example(void) {
    return PyModule_Create(&moduledef);
}
