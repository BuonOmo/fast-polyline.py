# Fast Polylines

[![CodSpeed](https://img.shields.io/endpoint?url=https://codspeed.io/badge.json)](https://app.codspeed.io/BuonOmo/fast-polyline.py?utm_source=badge)

Implementation of the [Google polyline algorithm][algorithm].

Up to 60x faster than the [polyline package][polyline].

## Benchmark

`make bench` made on a Apple M1 computer.

Obtaining file:///Users/ody/Dev/BuonOmo/fast-polyline.py
  Installing build dependencies: started
  Installing build dependencies: finished with status 'done'
  Checking if build backend supports build_editable: started
  Checking if build backend supports build_editable: finished with status 'done'
  Getting requirements to build editable: started
  Getting requirements to build editable: finished with status 'done'
  Preparing editable metadata (pyproject.toml): started
  Preparing editable metadata (pyproject.toml): finished with status 'done'
Requirement already satisfied: polyline~=2.0 in ./.venv/lib/python3.14/site-packages (from fast_polyline==0.3.0) (2.0.4)
Requirement already satisfied: pylint~=2.17 in ./.venv/lib/python3.14/site-packages (from fast_polyline==0.3.0) (2.17.7)
Requirement already satisfied: pytest~=7.4 in ./.venv/lib/python3.14/site-packages (from fast_polyline==0.3.0) (7.4.4)
Requirement already satisfied: commitizen~=3.5 in ./.venv/lib/python3.14/site-packages (from fast_polyline==0.3.0) (3.31.0)
Requirement already satisfied: argcomplete<3.6,>=1.12.1 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (3.5.3)
Requirement already satisfied: charset-normalizer<4,>=2.1.0 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (3.5.2)
Requirement already satisfied: colorama<0.5.0,>=0.4.1 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (0.4.6)
Requirement already satisfied: decli<0.7.0,>=0.6.0 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (0.6.3)
Requirement already satisfied: jinja2>=2.10.3 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (3.1.6)
Requirement already satisfied: packaging>=19 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (26.3)
Requirement already satisfied: pyyaml>=3.08 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (6.0.3)
Requirement already satisfied: questionary<3.0,>=2.0 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (2.1.1)
Requirement already satisfied: termcolor<3,>=1.1 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (2.5.0)
Requirement already satisfied: tomlkit<1.0.0,>=0.5.3 in ./.venv/lib/python3.14/site-packages (from commitizen~=3.5->fast_polyline==0.3.0) (0.15.1)
Requirement already satisfied: platformdirs>=2.2.0 in ./.venv/lib/python3.14/site-packages (from pylint~=2.17->fast_polyline==0.3.0) (4.12.3)
Requirement already satisfied: astroid<=2.17.0-dev0,>=2.15.8 in ./.venv/lib/python3.14/site-packages (from pylint~=2.17->fast_polyline==0.3.0) (2.15.8)
Requirement already satisfied: isort<6,>=4.2.5 in ./.venv/lib/python3.14/site-packages (from pylint~=2.17->fast_polyline==0.3.0) (5.13.2)
Requirement already satisfied: mccabe<0.8,>=0.6 in ./.venv/lib/python3.14/site-packages (from pylint~=2.17->fast_polyline==0.3.0) (0.7.0)
Requirement already satisfied: dill>=0.3.6 in ./.venv/lib/python3.14/site-packages (from pylint~=2.17->fast_polyline==0.3.0) (0.4.1)
Requirement already satisfied: lazy-object-proxy>=1.4.0 in ./.venv/lib/python3.14/site-packages (from astroid<=2.17.0-dev0,>=2.15.8->pylint~=2.17->fast_polyline==0.3.0) (1.12.0)
Requirement already satisfied: wrapt<2,>=1.14 in ./.venv/lib/python3.14/site-packages (from astroid<=2.17.0-dev0,>=2.15.8->pylint~=2.17->fast_polyline==0.3.0) (1.17.3)
Requirement already satisfied: iniconfig in ./.venv/lib/python3.14/site-packages (from pytest~=7.4->fast_polyline==0.3.0) (2.3.0)
Requirement already satisfied: pluggy<2.0,>=0.12 in ./.venv/lib/python3.14/site-packages (from pytest~=7.4->fast_polyline==0.3.0) (1.6.0)
Requirement already satisfied: prompt_toolkit<4.0,>=2.0 in ./.venv/lib/python3.14/site-packages (from questionary<3.0,>=2.0->commitizen~=3.5->fast_polyline==0.3.0) (3.0.53)
Requirement already satisfied: wcwidth>=0.1.4 in ./.venv/lib/python3.14/site-packages (from prompt_toolkit<4.0,>=2.0->questionary<3.0,>=2.0->commitizen~=3.5->fast_polyline==0.3.0) (0.9.2)
Requirement already satisfied: MarkupSafe>=2.0 in ./.venv/lib/python3.14/site-packages (from jinja2>=2.10.3->commitizen~=3.5->fast_polyline==0.3.0) (3.0.4)
Building wheels for collected packages: fast_polyline
  Building editable for fast_polyline (pyproject.toml): started
  Building editable for fast_polyline (pyproject.toml): finished with status 'done'
  Created wheel for fast_polyline: filename=fast_polyline-0.3.0-0.editable-cp314-cp314-macosx_11_0_arm64.whl size=2362 sha256=142063d6acc159291503f304d76f0ab2d13e316e4e2d75c7654ad9b93bfc1ea8
  Stored in directory: /private/var/folders/bm/n90p6z2x3ll96j8nt3chg8tw0000gn/T/pip-ephem-wheel-cache-fyr1hriy/wheels/77/4c/98/1cc5202aea8c3d121d907f03f989a0baf0cb3cc5bdb953db38
Successfully built fast_polyline
Installing collected packages: fast_polyline
  Attempting uninstall: fast_polyline
    Found existing installation: fast_polyline 0.2.0
    Uninstalling fast_polyline-0.2.0:
      Successfully uninstalled fast_polyline-0.2.0
Successfully installed fast_polyline-0.3.0

### encode

```
fast_polyline:	22.59ms
polyline:	555.44ms
fast_polyline is 24.6 times faster.
```

### decode

```
fast_polyline:	20.531ms
polyline:	360.65ms
fast_polyline is 17.6 times faster.
```


## Install

```bash
pip install fast_polyline
```

or in your `requirements.txt`:

```ruby
fast_polyline~=0.3.0
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
