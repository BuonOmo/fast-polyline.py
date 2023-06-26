.PHONY: install
install:
	python3 -m pip install -e .[dev]

.PHONY: bump
bump:
	@python3 -m commitizen  bump

.PHONY: bench
bench:
	@python3 perf/benchmark.py

.PHONY: configure
setup-clang:
	@echo "CompileFlags:\n  Add:\n$$(\
		pkg-config --cflags python-3.10 | tr ' ' "\0" | xargs -0 -I{} echo '    - {}'\
	)" > .clangd

.PHONY: console
console:
	python3 -ic 'import fast_polyline'

.PHONY: clean
clean:
	rm -rf build dist *.egg-info *.so
