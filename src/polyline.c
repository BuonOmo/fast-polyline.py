#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*
 * TODO: understand how to access a list of tuples.
 * TODO: handle errors
 * TODO: implement core of both functions
 * TODO: benchmark
 * TODO: readme (.rst???)
 * TODO: test
 */

// polyline.encode(points, precision=5)
static PyObject *polyline_encode(PyObject *self, PyObject *args) {
	int ok;
	PyObject *points;
	int precision = 5;
	ok = PyArg_ParseTuple(args, "O|i", &points, &precision);
	// https://docs.python.org/3/c-api/list.html
	ok = PyList_Check(&points);
	return PyUnicode_FromString("TODO: Implement polyline_encode");
}

// polyline.decode(polyline, precision=5)
static PyObject *polyline_decode(PyObject *self, PyObject *args) {
	int ok;
	const char *polyline;
	int precision = 5;
	ok = PyArg_ParseTuple(args, "s|i", &polyline, &precision);
	PyListObject *points = PyList_New(0);
	return (PyObject *)points;
}

static PyMethodDef methods[] = {
    {"encode", polyline_encode, METH_VARARGS, "Encode a polyline"},
    {"decode", polyline_decode, METH_VARARGS, "Decode a polyline"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {PyModuleDef_HEAD_INIT, "polyline",
                                    "Encode and decode polylines", -1, methods};
PyMODINIT_FUNC PyInit_polyline(void) { return PyModule_Create(&module); }
