#define PY_SSIZE_T_CLEAN
#include <Python.h>

/*
 * TODO: allow usage both of list and tuple.
 * TODO: handle errors correctly
 *.  - The Py*Check functions seems not to set the error message. Pedantic
 *python...
 * TODO: fix core of both functions
 *   - not all of the polyline is given, it seems trunked.
 * TODO: benchmark
 * TODO: readme (.rst???)
 * TODO: inline documentation, however it works in C Python.
 * TODO: test
 * TODO: offer merging with the existing polyline project
 * TODO: choose final naming (- or _ depending on how they do in python)
 */

// An encoded number can have at most _precision_ characters. However,
// it seems like we have a fuzzy behavior on low precisions. Hence a guard
// for those lower precisions.
#define MAX_ENCODED_CHUNKS(precision) (precision < 5 ? 5 : precision)

#define DEFAULT_PRECISION 5

// Already smaller than a sand grain. https://xkcd.com/2170/
// In fact, at 15 precision, we can be sure there will be precision loss
// in the C/Ruby value conversion. And 14 precision may be an edge case
// since we work with signed values.
#define MAX_PRECISION 13

#ifdef DEBUG
#define dbg(...) printf(__VA_ARGS__)
#else
#define dbg(...)
#endif

typedef unsigned int uint;

// Speed up the code a bit by not calling pow(10.0, precision) since it is
// always 10^precision. Convert that into a lookup instead of doing the
// calculation.
// The precision parameter is already checked in +_get_precision()+ to ensure
// that it is within the range.
static inline double _fast_pow10(uint precision) {
	const double lookup[MAX_PRECISION + 1] = {
	    1.0,    10.0,   1.0e+2, 1.0e+3,  1.0e+4,  1.0e+5,  1.0e+6,
	    1.0e+7, 1.0e+8, 1.0e+9, 1.0e+10, 1.0e+11, 1.0e+12, 1.0e+13};
	return lookup[precision];
}

static inline int _check_precision(int precision) {
	if (precision > MAX_PRECISION) {
		PyErr_SetString(PyExc_ValueError,
		                "precision too high (https://xkcd.com/2170/)");
		return 0;
	}
	if (precision < 0) {
		PyErr_SetString(PyExc_ValueError,
		                "negative precision doesn't make sense");
		return 0;
	}
	return 1;
}

// polyline.decode(polyline, precision=5)
static PyObject *polyline_decode(PyObject *self, PyObject *args) {
	const char *polyline;
	int precision = DEFAULT_PRECISION;
	if (!PyArg_ParseTuple(args, "s|i", &polyline, &precision)) return NULL;
	if (!_check_precision(precision)) return NULL;
	double precision_value = _fast_pow10(precision);
	PyObject *ary = PyList_New(0);
	// Helps keeping track of whether we are computing lat (0) or lng (1).
	uint8_t index = 0;
	size_t shift = 0;
	int64_t delta = 0;
	PyObject *pair[2];
	double latlng[2] = {0.0, 0.0};
	// Loops until end of string nul character is encountered.
	while (*polyline) {
		int64_t chunk = *polyline++;

		if (chunk < 63 || chunk > 126) {
			char buffer[64];
			sprintf(buffer, "invalid character '%c'", (char)chunk);
			PyErr_SetString(PyExc_ValueError, buffer);
			return NULL;  // TODO: ensure that the objects we created will be
			              // gc'd
		}

		chunk -= 63;
		delta = delta | ((chunk & ~0x20) << shift);
		shift += 5;

		if (!(chunk & 0x20)) {
			delta = (delta & 1) ? ~(delta >> 1) : (delta >> 1);
			latlng[index] += delta;
			pair[index] =
			    PyFloat_FromDouble((double)latlng[index] / precision_value);
			// When both coordinates are parsed, we can push those to the result
			// ary.
			if (index) PyList_Append(ary, PyTuple_Pack(2, pair[0], pair[1]));
			// Reinitilize since we are done for current coordinate.
			index = 1 - index;
			delta = 0;
			shift = 0;
		}
	}
	return ary;
}

static inline uint8_t _polyline_encode_number(char *chunks, int64_t number) {
	dbg("_polyline_encode_number(\"%s\", %lli)\n", chunks, number);
	number = number < 0 ? ~(number << 1) : (number << 1);
	uint8_t i = 0;
	while (number >= 0x20) {
		uint8_t chunk = number & 0x1f;
		chunks[i++] = (0x20 | chunk) + 63;
		number = number >> 5;
	}
	chunks[i++] = number + 63;
	dbg("%u encoded chunks\n", i);
	dbg("chunks: '%s'\n", chunks);
	dbg("/_polyline_encode_number\n");
	return i;
}

// polyline.encode(points, precision=5)
static PyObject *polyline_encode(PyObject *self, PyObject *args) {
	PyObject *ary;
	// Dynamically checked for [0; 13] inclusion.
	int precision = DEFAULT_PRECISION;
	if (!PyArg_ParseTuple(args, "O|i", &ary, &precision)) return NULL;
	if (!_check_precision(precision)) return NULL;
	double precision_value = _fast_pow10(precision);
	if (!PyList_Check(ary)) return NULL;

	size_t len = PyList_Size(ary);
	uint64_t i;
	PyObject *current_pair;
	int64_t prev_pair[2] = {0, 0};

	char *chunks =
	    malloc(MAX_ENCODED_CHUNKS(precision) * 2 * len * sizeof(char));
	size_t chunks_index = 0;
	dbg("ary.len: %lu\n", len);
	for (i = 0; i < len; i++) {
		current_pair = PyList_GetItem(ary, i);
		uint8_t j;
		if (PyList_Check(current_pair))
			current_pair = PyList_AsTuple(current_pair);
		if (!PyTuple_Check(current_pair)) {
			free(chunks);
			return NULL;
		}
		if (PyTuple_Size(current_pair) != 2) {
			free(chunks);
			PyErr_SetString(PyExc_ValueError, "wrong number of coordinates");
			return NULL;
		}
		for (j = 0; j < 2; j++) {
			PyObject *current_coord = PyTuple_GetItem(current_pair, j);
			if (!PyFloat_Check(current_coord)) {
				free(chunks);
				return NULL;
			}
			double coord = PyFloat_AsDouble(current_coord);
			if (-180.0 > coord || coord > 180.0) {
				free(chunks);
				PyErr_SetString(PyExc_ValueError,
				                "coordinates must be between -180.0 and 180.0");
				return NULL;
			}
			int64_t rounded_value = round(coord * precision_value);
			int64_t delta = rounded_value - prev_pair[j];
			prev_pair[j] = rounded_value;
			// We pass a pointer to the current chunk that need to be filled.
			// Doing so avoid having to copy the string every single iteration.
			chunks_index += _polyline_encode_number(
			    chunks + chunks_index * sizeof(char), delta);
		}
	}

	PyObject *polyline = PyUnicode_FromStringAndSize(chunks, chunks_index);
	free(chunks);
	return polyline;
}

static PyMethodDef methods[] = {
    {"decode", polyline_decode, METH_VARARGS, "Decode a polyline"},
    {"encode", polyline_encode, METH_VARARGS, "Encode a polyline"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {PyModuleDef_HEAD_INIT, "polyline",
                                    "Encode and decode polylines", -1, methods};
PyMODINIT_FUNC PyInit_polyline(void) { return PyModule_Create(&module); }
