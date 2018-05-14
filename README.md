Max Bandwidth (maxb)
====================
A utility for testing maximum bandwidth between systems.

The current project page is located here: <https://github.com/bbrice/maxb>

Building
--------
A Visual Studio 2017 project is provided for building on Windows and a
Makefile is provided for building on GNU-compatible systems.

Usage
-----
At the moment, this application supports testing for UDP bandwidth.

1. Benchmark how much data can be transferred within a specific time period.
The time period is specified in seconds.  In this example, 10 seconds is used:

		maxb 10.28.1.130 10

License
-------
`maxb` is licensed under the GPL v2 license. See [LICENSE.txt](LICENSE.txt)
for more info.
