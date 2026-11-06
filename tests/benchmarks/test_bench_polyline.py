"""
CodSpeed benchmarks for fast_polyline.

Run locally with `pytest tests/benchmarks --codspeed`.
"""

import sys
from pathlib import Path
from random import Random

import pytest

import fast_polyline

# Reuse the Paris route from the comparison benchmark in perf/benchmark.py.
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from perf.benchmark import POINTS as PARIS_POINTS  # noqa: E402
from perf.benchmark import POLYLINE as PARIS_POLYLINE  # noqa: E402

OFFICIAL_POINTS = [(38.5, -120.2), (40.7, -120.95), (43.252, -126.453)]
OFFICIAL_POLYLINE = '_p~iF~ps|U_ulLnnqC_mqNvxq`@'


def _random_walk(count, seed=42):
	"""Deterministic, GPS-trace-like list of coordinates."""
	rng = Random(seed)
	lat, lng = 48.85726, 2.35238
	points = []
	for _ in range(count):
		lat += rng.uniform(-0.01, 0.01)
		lng += rng.uniform(-0.01, 0.01)
		points.append((lat, lng))
	return points


LARGE_POINTS = _random_walk(10_000)
LARGE_POINTS_AS_LISTS = [list(p) for p in LARGE_POINTS]
LARGE_POLYLINE = fast_polyline.encode(LARGE_POINTS)
LARGE_POLYLINE_P6 = fast_polyline.encode(LARGE_POINTS, 6)


@pytest.mark.benchmark
def test_encode_official_example():
	fast_polyline.encode(OFFICIAL_POINTS)


@pytest.mark.benchmark
def test_decode_official_example():
	fast_polyline.decode(OFFICIAL_POLYLINE)


@pytest.mark.benchmark
def test_encode_paris_route():
	fast_polyline.encode(PARIS_POINTS)


@pytest.mark.benchmark
def test_decode_paris_route():
	fast_polyline.decode(PARIS_POLYLINE)


@pytest.mark.parametrize('size', [100, 1_000, 10_000])
def test_encode_random_walk(benchmark, size):
	points = LARGE_POINTS[:size]
	result = benchmark(fast_polyline.encode, points)
	assert isinstance(result, str)


@pytest.mark.parametrize('size', [100, 1_000, 10_000])
def test_decode_random_walk(benchmark, size):
	polyline = fast_polyline.encode(LARGE_POINTS[:size])
	result = benchmark(fast_polyline.decode, polyline)
	assert len(result) == size


def test_encode_list_of_lists(benchmark):
	benchmark(fast_polyline.encode, LARGE_POINTS_AS_LISTS)


def test_encode_precision_6(benchmark):
	benchmark(fast_polyline.encode, LARGE_POINTS, 6)


def test_decode_precision_6(benchmark):
	result = benchmark(fast_polyline.decode, LARGE_POLYLINE_P6, 6)
	assert len(result) == len(LARGE_POINTS)


def test_roundtrip(benchmark):
	def roundtrip():
		return fast_polyline.decode(fast_polyline.encode(LARGE_POINTS))

	result = benchmark(roundtrip)
	assert len(result) == len(LARGE_POINTS)
