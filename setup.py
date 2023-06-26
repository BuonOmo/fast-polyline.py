"""
Setup for the C extension binding.
"""

from setuptools import setup, Extension

setup(ext_modules=[Extension("fast_polyline", sources=["src/polyline.c"])])
