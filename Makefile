ALL_TARGETS = $(wildcard src/*.c) pyproject.toml setup.py
PYTHON = @python3

##@ Utility

FMT_TITLE='\\033[7\;1m'
FMT_PRIMARY='\\033[36m'
FMT_END='\\033[0m'
.PHONY: help
help: ## Shows this help menu
	@printf -- "                               FAST-POLYLINE\n"
	@printf -- "---------------------------------------------------------------------------\n"
	@awk ' \
			BEGIN {FS = ":.*##"; printf "Usage: make ${FMT_PRIMARY}<target>${FMT_END}\n"} \
	    	/^[a-zA-Z0-9_-]+:.*?##/ { printf "  $(FMT_PRIMARY)%-30s$(FMT_END) %s\n", $$1, $$2 } \
			/^##@/ { printf "\n$(FMT_TITLE) %s $(FMT_END)\n", substr($$0, 5) } \
		' $(MAKEFILE_LIST)

.PHONY: console
console: ext ## Runs the Python REPL with fast_polyline
	$(PYTHON) -ic 'import fast_polyline'

.PHONY: test
test: ext ## Runs tests
	$(PYTHON) -m pytest -q

.PHONY: lint
lint: ## Checks python syntax
	$(PYTHON) -m pylint *.py perf/*.py

.PHONY: bench
bench: ext ## Run the benchmark
	$(PYTHON) perf/benchmark.py

.PHONY: bump ## Bump version and update changelog
bump:
	$(PYTHON) -m commitizen bump

README.md: bump bin/generate_readme bin/resources/README.md.in pyproject.toml perf/benchmark.py $(ALL_TARGETS)
	bin/generate_readme
	git add README.md
	git commit --amend --no-edit

.PHONY: publish
publish: test README.md ## Publish to pypi.org (TODO)
	@echo TODO

##@ C extension

src/fast_polyline_ext.*.so: $(ALL_TARGETS)
	$(PYTHON) -m pip install -e .[dev]

.PHONY: ext
ext: src/fast_polyline_ext.*.so ## Compiles the C extension

.clangd:
	@echo "CompileFlags:\n  Add:\n$$(\
		pkg-config --cflags python-3.10 | tr ' ' "\0" | xargs -0 -I{} echo '    - {}'\
	)" > .clangd

.PHONY: setup-clang
setup-clang: .clangd # Setup clangd for C/C++ development

.PHONY: clean
clean: ## Cleans compiled stuff
	rm -rf build dist **/*.egg-info **/*.so
