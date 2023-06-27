"""
Fast & Easy Google Polylines

Implementation of the `Google polyline algorithm
<https://code.google.com/apis/maps/documentation/utilities/polylinealgorithm.html>`_

.. code-block:: language-python
    fast_polyline.encode([[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]])
    # => '_p~iF~ps|U_ulLnnqC_mqNvxq`@'

    fast_polyline.decode('_p~iF~ps|U_ulLnnqC_mqNvxq`@')
    # => [[38.5, -120.2], [40.7, -120.95], [43.252, -126.453]]

You can set an arbitrary precision for your coordinates to be
encoded/decoded. It may be from 1 to 13 decimal digits. However,
13 may be `too much
<https://www.explainxkcd.com/wiki/index.php/2170:_Coordinate_Precision>`_.

.. image:: https://imgs.xkcd.com/comics/coordinate_precision.png
        :alt: Coordinate Precision explained by XKC
"""

import fast_polyline_ext

def encode(coordinates, precision=5):
	"""
	Encode a list of (lat, lng) coordinates
	into a Polyline
	"""
	return fast_polyline_ext.encode(coordinates, precision)

def decode(polyline, precision=5):
	"""
	Decode a Polyline into a list of (lat, lng)
	coordinate tuples
	"""
	return fast_polyline_ext.decode(polyline, precision)

__all__ = ['encode', 'decode']
