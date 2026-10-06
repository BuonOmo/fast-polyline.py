import time
from random import randint, uniform

from pytest import raises

import fast_polyline as polyline


def _assert_no_leaked_blocks(fn, iterations=10_000, tolerance=50):
	"""Runs fn() `iterations` times and asserts that the number of
	allocator blocks (sys.getallocatedblocks()) hasn't grown beyond a
	small tolerance.

	NOTE: gc.get_objects()/gc.collect() are NOT reliable here. CPython
	can leave tuples made entirely of non-container, immutable items
	(floats, ints, None, ...) untracked by the cyclic GC, so a C-level
	refcount leak of such objects is invisible to gc.get_objects() even
	though real memory is leaked. sys.getallocatedblocks() counts raw
	pymalloc blocks and reliably reflects this kind of leak.
	"""
	import gc
	import sys

	_ = gc.collect()
	before = sys.getallocatedblocks()

	for _ in range(iterations):
		fn()

	_ = gc.collect()
	after = sys.getallocatedblocks()

	assert after <= (before + tolerance), (
		f'Allocated block count grew from {before} to {after} over {iterations} '
		'iterations; objects were not released'
	)


def test_no_leak_decode_error():
	def run():
		with raises(ValueError, match="invalid character '='"):
			polyline.decode('gu`wFnfys@???nKgE??gE?????oK????fE??fE=')

	_assert_no_leaked_blocks(run)


def test_no_leak_encode_error():
	bad_points = [
		(40.641, -8.654),
		(40.641, -8.654),
		(40.641, -8.656),
		(40.642, -8.656),
		(40.642, -8.655),
		(40.642, -8.655),
		(40.642, -8.655),
		(1, None),
	]

	def run():
		with raises(TypeError, match=r'points must be a list of \(lat, lng\) pairs'):
			polyline.encode(bad_points)

	_assert_no_leaked_blocks(run)


def test_no_leak_decode_success():
	def run():
		d = polyline.decode('gu`wFnfys@???nKgE??gE?????oK????fE??fE')
		assert len(d) == 12

	_assert_no_leaked_blocks(run)


def test_no_leak_encode_success():
	points = [
		(40.641, -8.654),
		(40.641, -8.654),
		(40.641, -8.656),
		(40.642, -8.656),
	]

	def run():
		e = polyline.encode(points)
		assert isinstance(e, str)

	_assert_no_leaked_blocks(run)


def test_no_leak_encode_list_of_lists():
	points = [[40.641, -8.654], [40.641, -8.656], [40.642, -8.656]]

	def run():
		e = polyline.encode(points)
		assert isinstance(e, str)

	_assert_no_leaked_blocks(run)


def test_no_leak_encode_list_of_lists_error():
	bad_points = [[40.641, -8.654], [1, None]]

	def run():
		with raises(TypeError, match=r'points must be a list of \(lat, lng\) pairs'):
			polyline.encode(bad_points)

	_assert_no_leaked_blocks(run)


def test_decode_multiple_points():
	d = polyline.decode('gu`wFnfys@???nKgE??gE?????oK????fE??fE')
	assert d == [
		(40.641, -8.654),
		(40.641, -8.654),
		(40.641, -8.656),
		(40.642, -8.656),
		(40.642, -8.655),
		(40.642, -8.655),
		(40.642, -8.655),
		(40.642, -8.653),
		(40.642, -8.653),
		(40.642, -8.653),
		(40.641, -8.653),
		(40.641, -8.654),
	]


def test_decode_multiple_points_precision():
	d = polyline.decode('_epolA~ieoOnF??~{Bo}@??o}@?????_|B????n}@??n}@', 6)
	assert d == [
		(40.64112, -8.654),
		(40.641, -8.654),
		(40.641, -8.656),
		(40.642, -8.656),
		(40.642, -8.655),
		(40.642, -8.655),
		(40.642, -8.655),
		(40.642, -8.653),
		(40.642, -8.653),
		(40.642, -8.653),
		(40.641, -8.653),
		(40.641, -8.654),
	]


def test_decode_official_example():
	d = polyline.decode('_p~iF~ps|U_ulLnnqC_mqNvxq`@')
	assert d == [(38.500, -120.200), (40.700, -120.950), (43.252, -126.453)]


def test_decode_official_example_precision():
	d = polyline.decode('_izlhA~rlgdF_{geC~ywl@_kwzCn`{nI', 6)
	assert d == [(38.500, -120.200), (40.700, -120.950), (43.252, -126.453)]


def test_decode_single_point():
	d = polyline.decode('gu`wFf`ys@')
	assert d == [(40.641, -8.653)]


def test_decode_single_point_precision():
	d = polyline.decode('o}oolAnkcoO', 6)
	assert d == [(40.641, -8.653)]


def test_encode_multiple_points():
	e = polyline.encode(
		[
			(40.641, -8.654),
			(40.641, -8.654),
			(40.641, -8.656),
			(40.642, -8.656),
			(40.642, -8.655),
			(40.642, -8.655),
			(40.642, -8.655),
			(40.642, -8.653),
			(40.642, -8.653),
			(40.642, -8.653),
			(40.641, -8.653),
			(40.641, -8.654),
		]
	)
	assert e == 'gu`wFnfys@???nKgE??gE?????oK????fE??fE'


def test_encode_multiple_points_precision():
	e = polyline.encode(
		[
			(40.64112345, -8.654),
			(40.641, -8.654),
			(40.641, -8.656),
			(40.642, -8.656),
			(40.642, -8.655),
			(40.642, -8.655),
			(40.642, -8.655),
			(40.642, -8.653),
			(40.642, -8.653),
			(40.642, -8.653),
			(40.641, -8.653),
			(40.641, -8.654),
		],
		6,
	)
	assert e == 'eepolA~ieoOtF??~{Bo}@??o}@?????_|B????n}@??n}@'


def test_encode_official_example():
	e = polyline.encode([(38.500, -120.200), (40.700, -120.950), (43.252, -126.453)])
	assert e == '_p~iF~ps|U_ulLnnqC_mqNvxq`@'


def test_encode_official_example_precision():
	e = polyline.encode([(38.500, -120.200), (40.700, -120.950), (43.252, -126.453)], 6)
	assert e == '_izlhA~rlgdF_{geC~ywl@_kwzCn`{nI'


def test_encode_single_point():
	e = polyline.encode([(40.64155, -8.65344)])
	assert e == 'ux`wF~bys@'

	e = polyline.encode([(40.641552, -8.653441)])
	assert e == 'ux`wF~bys@'


def test_encode_single_point_rounding():
	e = polyline.encode([(0, 0.000006), (0, 0.000002)])
	assert e == '?A?@'


def test_rounding_py3_match_py2():
	e = polyline.encode(
		[(36.05322, -112.084004), (36.053573, -112.083914), (36.053845, -112.083965)]
	)
	assert e == 'ss`{E~kbkTeAQw@J'


def test_encode_single_point_precision():
	e = polyline.encode([(40.641123, -8.653321)], 6)
	assert e == 'eepolAp_doO'

	e = polyline.encode([(40.6411233123, -8.6533214234)], 6)
	assert e == 'eepolAp_doO'


def test_encode_accepts_list_of_lists():
	tuples = [(40.641, -8.654), (40.641, -8.656), (40.642, -8.656)]
	lists = [list(p) for p in tuples]

	assert polyline.encode(lists) == polyline.encode(tuples)


def test_encode_integer_coordinates():
	assert polyline.encode([(40, -8), (41, -9)]) == polyline.encode(
		[(40.0, -8.0), (41.0, -9.0)]
	)


def test_encode_empty_list():
	assert polyline.encode([]) == ''


def test_decode_empty_string():
	assert polyline.decode('') == []


def test_a_variety_of_precisions():
	"""uses a generator to create a variety of lat-lon's across the global
	and tests a range of precision settings from 2 to 9"""

	def generator():
		while True:
			coords = []
			for i in range(2, randint(4, 10)):
				lat, lon = uniform(-180.0, 180.0), uniform(-180.0, 180.0)
				coords.append((lat, lon))
			yield coords

	patience = 0.5  # seconds.
	waypoints, okays = 0, 0

	g = generator()
	start = time.time()
	while time.time() < start + patience:
		precision = randint(2, 9)
		wp = next(g)
		waypoints += len(wp)
		poly = polyline.encode(wp, precision)
		wp2 = polyline.decode(poly, precision)
		if wp == wp2:
			okays += len(wp2)
		else:
			for idx, _ in enumerate(wp):
				dx, dy = abs(wp[idx][0] - wp2[idx][0]), abs(wp[idx][1] - wp2[idx][1])
				if dx > 10 ** -(precision - 1) or dy > 10 ** -(precision - 1):
					print(f'idx={idx}, dx={dx}, dy={dy}')
				else:
					okays += 1

	assert okays == waypoints
	print(
		f'encoded and decoded {100 * okays / float(waypoints):.2f}% correctly for {waypoints} '
		f'waypoints @ {round(waypoints / patience, 0)} wp/sec'
	)
