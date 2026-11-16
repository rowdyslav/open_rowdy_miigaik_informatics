#include <Python.h>

#include <ctime>

static PyObject* make_result(PyObject* result_list, double elapsed_seconds) {
    PyObject* elapsed = PyFloat_FromDouble(elapsed_seconds);
    if (!elapsed) {
        Py_DECREF(result_list);
        return NULL;
    }

    PyObject* result = PyTuple_New(2);
    if (!result) {
        Py_DECREF(result_list);
        Py_DECREF(elapsed);
        return NULL;
    }

    PyTuple_SET_ITEM(result, 0, result_list);
    PyTuple_SET_ITEM(result, 1, elapsed);
    return result;
}

static PyObject* core_divide(PyObject*, PyObject* args) {
    PyObject* vector;
    double divisor;

    if (!PyArg_ParseTuple(args, "Od:divide", &vector, &divisor)) {
        return NULL;
    }

    if (!PyList_Check(vector)) {
        PyErr_SetString(PyExc_TypeError, "vector must be a list");
        return NULL;
    }

    if (divisor == 0.0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "divisor must not be zero");
        return NULL;
    }

    const Py_ssize_t size = PyList_Size(vector);
    if (size < 0) {
        return NULL;
    }

    if (size == 0) {
        PyObject* empty = PyList_New(0);
        if (!empty) {
            return NULL;
        }
        return make_result(empty, 0.0);
    }

    PyObject* result_list = PyList_New(size);
    if (!result_list) {
        return NULL;
    }

    const std::clock_t start = std::clock();
    for (Py_ssize_t index = 0; index < size; ++index) {
        PyObject* item = PyList_GetItem(vector, index);
        if (!item) {
            Py_DECREF(result_list);
            return NULL;
        }

        const double value = PyFloat_AsDouble(item);
        if (PyErr_Occurred()) {
            Py_DECREF(result_list);
            return NULL;
        }

        PyObject* result_item = PyFloat_FromDouble(value / divisor);
        if (!result_item || PyErr_Occurred()) {
            Py_XDECREF(result_item);
            Py_DECREF(result_list);
            return NULL;
        }

        PyList_SET_ITEM(result_list, index, result_item);
    }
    const std::clock_t finish = std::clock();

    const double elapsed_seconds = static_cast<double>(finish - start) / CLOCKS_PER_SEC;
    return make_result(result_list, elapsed_seconds);
}

static PyMethodDef methods[] = {
    {"divide", core_divide, METH_VARARGS, "Divide a Python list by a number."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "_core",
    "C++ vector division through Python/C API",
    -1,
    methods
};

PyMODINIT_FUNC PyInit__core() {
    return PyModule_Create(&module);
}
