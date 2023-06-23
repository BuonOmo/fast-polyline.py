.PHONY: install
install:
	python3 -m pip install -e .

.PHONY: console
console: install
	python3 -ic 'import polyline'
