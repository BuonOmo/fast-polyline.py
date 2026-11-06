#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdint.h>

/* TODO: https://packaging.python.org/en/latest/tutorials/packaging-projects/ */

// An encoded number can have at most _precision_ characters. However,
// it seems like we have a fuzzy behavior on low precisions. Hence a guard
// for those lower precisions.
#define MAX_ENCODED_CHUNKS(precision) (precision < 6 ? 6 : precision)

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

// Ensure that precision is between 0 and 13, set error string
// otherwise.
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

// Wrapper around PyArg_ParseTuple that sets an error message.
static inline int _check_args(PyObject *args, const char *fmt, ...) {
	// Passing through variadic arguments is not that easy in C.
	// Here's some information on the topic:
	// https://c-faq.com/varargs/handoff.html
	va_list argp;
	va_start(argp, fmt);
	int ret = PyArg_VaParse(args, fmt, argp);
	va_end(argp);
	if (!ret) PyErr_SetString(PyExc_TypeError, "invalid arguments");
	return ret;
}

// polyline.decode(polyline, precision)
static PyObject *polyline_decode(PyObject *self, PyObject *args) {
	const char *polyline;
	int precision;
	if (!_check_args(args, "si", &polyline, &precision)) return NULL;
	if (!_check_precision(precision)) return NULL;
	double precision_value = _fast_pow10(precision);

	// Count how many (lat, lng) pairs this polyline will decode into, so the
	// result list can be allocated exactly once instead of growing it (and
	// potentially reallocating/copying its backing array) via repeated
	// PyList_Append calls below. A number ends on a byte whose continuation
	// bit (0x20) is unset, and a pair needs two such numbers. Any bytes past
	// the first invalid character can only inflate this count (never make it
	// undercount what the parser below will actually produce before it
	// bails out on that same invalid character), so it's always a safe
	// upper bound, and an exact count for well-formed input.
	size_t num_count = 0;
	for (const unsigned char *p = (const unsigned char *)polyline; *p; p++) {
		if (!((*p - 63) & 0x20)) num_count++;
	}
	Py_ssize_t num_pairs = (Py_ssize_t)(num_count / 2);
	PyObject *ary = PyList_New(num_pairs);
	if (ary == NULL) return NULL;
	Py_ssize_t pair_count = 0;
	// Helps keeping track of whether we are computing lat (0) or lng (1).
	uint8_t index = 0;
	size_t shift = 0;
	int64_t delta = 0;
	PyObject *pair[2] = {NULL, NULL};
	double latlng[2] = {0.0, 0.0};
	// Loops until end of string nul character is encountered.
	while (*polyline) {
		int64_t chunk = *polyline++;

		if (chunk < 63 || chunk > 126) {
			PyErr_Format(PyExc_ValueError, "invalid character '%c'",
			             (char)chunk);
			Py_XDECREF(pair[0]);
			// ary may still have unfilled (NULL) slots past pair_count; that's
			// fine, list deallocation already Py_XDECREFs every slot safely.
			Py_DECREF(ary);
			return NULL;
		}

		chunk -= 63;
		delta = delta | ((chunk & ~0x20) << shift);
		shift += 5;

		if (!(chunk & 0x20)) {
			delta = (delta & 1) ? ~(delta >> 1) : (delta >> 1);
			latlng[index] += delta;
			pair[index] =
			    PyFloat_FromDouble((double)latlng[index] / precision_value);
			if (pair[index] == NULL) {
				Py_XDECREF(pair[1 - index]);
				Py_DECREF(ary);
				return NULL;
			}
			// When both coordinates are parsed, we can push those to the result
			// ary.
			if (index) {
				PyObject *point = PyTuple_New(2);
				if (point == NULL) {
					Py_DECREF(pair[0]);
					Py_DECREF(pair[1]);
					Py_DECREF(ary);
					return NULL;
				}
				// PyTuple_SET_ITEM steals the reference, avoiding the extra
				// incref/decref pair that PyTuple_Pack would otherwise cost.
				PyTuple_SET_ITEM(point, 0, pair[0]);
				PyTuple_SET_ITEM(point, 1, pair[1]);
				pair[0] = pair[1] = NULL;
				// ary was preallocated with num_pairs (NULL) slots above, and
				// pair_count never exceeds num_pairs, so this is safe and
				// steals the reference without bounds-checking overhead.
				PyList_SET_ITEM(ary, pair_count++, point);
			}
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
	int precision;
	if (!_check_args(args, "Oi", &ary, &precision)) return NULL;
	if (!_check_precision(precision)) return NULL;
	double precision_value = _fast_pow10(precision);
	if (!PyList_Check(ary)) {
		PyErr_SetString(PyExc_TypeError, "points must be a list");
		return NULL;
	}

	size_t len = PyList_Size(ary);
	uint64_t i;
	int64_t prev_pair[2] = {0, 0};

	size_t chunk_size = (size_t)MAX_ENCODED_CHUNKS(precision) * 2;
	if (len != 0 && chunk_size > SIZE_MAX / len) {
		PyErr_SetString(PyExc_OverflowError, "points list too large to encode");
		return NULL;
	}
	size_t alloc_size = chunk_size * len;
	dbg("allocated size: %u * 2 * %lu = %lu\n", MAX_ENCODED_CHUNKS(precision),
	    len, alloc_size);
	char *chunks = alloc_size ? malloc(alloc_size * sizeof(char)) : NULL;
	if (alloc_size && chunks == NULL) return PyErr_NoMemory();
	size_t chunks_index = 0;
	dbg("ary.len: %lu\n", len);
	for (i = 0; i < len; i++) {
		// ary was already confirmed to be a list and i < len, so this is
		// safe and skips PyList_GetItem's redundant bounds check.
		PyObject *current_pair = PyList_GET_ITEM(ary, i);
		uint8_t j;
		// PySequence_Fast only copies into a new list when current_pair isn't
		// already a list/tuple; for the common list-of-lists/tuples inputs it
		// just returns a new reference to the same object, unlike the
		// previous PyList_AsTuple() call which always allocated a fresh
		// 2-item tuple for list inputs.
		PyObject *fast_pair = PySequence_Fast(
		    current_pair, "points must be a list of (lat, lng) pairs");
		if (fast_pair == NULL) goto error;
		if (PySequence_Fast_GET_SIZE(fast_pair) != 2) {
			Py_DECREF(fast_pair);
			goto points_error;
		}

		for (j = 0; j < 2; j++) {
			PyObject *current_coord = PySequence_Fast_GET_ITEM(fast_pair, j);
			int is_float = PyFloat_Check(current_coord);
			int is_long = PyLong_Check(current_coord);
			if (!is_float && !is_long) {
				Py_DECREF(fast_pair);
				goto points_error;
			}

			double coord;
			if (is_float) {
				// current_coord is already confirmed to be a float, so read it
				// directly instead of paying for PyFloat_AsDouble's checks.
				coord = PyFloat_AS_DOUBLE(current_coord);
			} else {
				coord = PyLong_AsDouble(current_coord);
				if (coord == -1.0 && PyErr_Occurred()) {
					Py_DECREF(fast_pair);
					free(chunks);
					return NULL;
				}
			}
			if (-180.0 > coord || coord > 180.0) {
				Py_DECREF(fast_pair);
				goto points_error;
			}

			int64_t rounded_value = round(coord * precision_value);
			int64_t delta = rounded_value - prev_pair[j];
			prev_pair[j] = rounded_value;
			// We pass a pointer to the current chunk that need to be filled.
			// Doing so avoid having to copy the string every single iteration.
			chunks_index += _polyline_encode_number(
			    chunks + chunks_index * sizeof(char), delta);
		}
		Py_DECREF(fast_pair);
	}

	assert(chunks_index < alloc_size);
	dbg("final chunks_index: %lu\n", chunks_index);
	PyObject *polyline = PyUnicode_FromStringAndSize(chunks, chunks_index);
	free(chunks);
	return polyline;

points_error:
	PyErr_SetString(PyExc_TypeError,
	                "points must be a list of (lat, lng) pairs");
error:
	free(chunks);
	return NULL;
}

static PyMethodDef methods[] = {{"decode", polyline_decode, METH_VARARGS, ""},
                                {"encode", polyline_encode, METH_VARARGS, ""},
                                {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {PyModuleDef_HEAD_INIT, "fast_polyline_ext",
                                    "", -1, methods};
PyMODINIT_FUNC PyInit_fast_polyline_ext(void) {
	return PyModule_Create(&module);
}
