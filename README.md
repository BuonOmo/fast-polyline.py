# Fast Polylines

Implementation of the [Google polyline algorithm][algorithm].

Up to 60x faster than the [polyline package][polyline].

## Benchmark

`make bench` made on a Apple M1 computer.


### encode

```
fast_polyline:	10.098ms
polyline:	613.31ms
fast_polyline is 60.7 times faster.
```

### decode

```
fast_polyline:	24.245ms
polyline:	425.76ms
fast_polyline is 17.6 times faster.
```


## Install

```bash
pip install fast_polyline
```

or in your `requirements.txt`:

```ruby
fast_polyline~=0.1.0
```

## Usage

```python
import fast_polyline

fast_polyline.encode([[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]])
# => "_p~iF~ps|U_ulLnnqC_mqNvxq`@"

fast_polyline.decode("_p~iF~ps|U_ulLnnqC_mqNvxq`@")
# => [[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]]
```

## Advanced usage

**Use a different precision**

Default precision is `5` decimals, to use a precision of `6`:
```ruby
fast_polyline.encode([[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]], 6)
# => "_izlhA~rlgdF_{geC~ywl@_kwzCn`{nI"

fast_polyline.decode("_izlhA~rlgdF_{geC~ywl@_kwzCn`{nI", 6)
# => [[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]]
```
The precision max is `13`. And I advanced against using it
since [it is already way too precise][xkcd].

`Encoder` and `Decoder` modules are deprec

## Contributing

```bash
git clone git@github.com:BuonOmo/fast-polyline.py
cd fast-polyline.py
make # show all commands you might need
# Implement a feature, resolve a bug...
make lint
make test
git commit "feat: respect conventional commits"
# Make a PR
```

[And here's the Python C ABI doc.][python-c]

## License

Please see the LICENSE file.

[algorithm]: https://code.google.com/apis/maps/documentation/utilities/polylinealgorithm.html
[polyline]: https://github.com/frederickjansen/polyline
[xkcd]: https://xkcd.com/2170/
[python-c]: https://docs.python.org/3/c-api/index.html
