Max Bandwidth (maxb)
====================
A utility for testing maximum bandwidth between systems.

The current project page is located here: <https://github.com/bbrice/maxb>

Building
--------
A Visual Studio 2013.5 project is provided for building on Windows and a
Makefile is provided for building on GNU-compatible systems.

Usage
-----
At the moment, this application supports testing for UDP bandwidth.  There are
only two modes for running.

1. Benchmark how long it takes to send 100 MB.

	maxb 10.28.1.138

2. Benchmark how much data can be transferred within a specific time period.
The time period is specified in seconds.  In this example, 10 seconds is used.

	maxb 10.28.1.130 10

License
-------
`maxb` is licensed under the GPL v2 license. See [LICENSE.txt](LICENSE.txt)
for more info.
