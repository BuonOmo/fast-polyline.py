"""
Setup for the C extension binding.
"""

from setuptools import setup, Extension

setup(ext_modules=[Extension("fast_polyline_ext", sources=["src/fast_polyline_ext.c"])])
