.PHONY: install
install:
	python3 -m pip install -e .

.PHONY: configure
configure: .clangd

.PHONY: console
console: install
	python3 -ic 'import polyline'

.clangd:
	@echo "CompileFlags:\n  Add:\n$$(\
		pkg-config --cflags python-3.10 | tr ' ' "\0" | xargs -0 -I{} echo '    - {}'\
	)" > .clangd
