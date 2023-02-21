# Max Bandwidth (maxb)

A utility for testing maximum bandwidth between systems.

The current project page is located here: <https://github.com/blb2/maxb>.


## Building

CMake is used to build this project. There are plenty of ways of using CMake, so
choose the way that you prefer to work best.


### Building on Command-line

If `cmake` is not in your system's `PATH`, then feel free to add it temporarily
with one of the following commands:

    PATH=%PROGRAMFILES%\CMake\bin;%PATH%
    PATH=$(cygpath -u "$PROGRAMFILES")/CMake/bin:$PATH

The following are steps taken to produce your first build:

1.  Initialize CMake from the project's root directory:

        cmake -B build

    To have CMake be verbose when building, use something like this:

        cmake -B build -D CMAKE_VERBOSE_MAKEFILE=ON

    Of course, when running the `--build` command later on, you can specify `-v`
    to `cmake` and it will probably accomplish the same thing.

    CMake will choose the native platform of the host machine when building. You
    can change that with the `-A` argument. For example, you're on a 64-bit
    Windows platform, but wish to build for 32-bit:

        cmake -B build -A Win32

2.  The following are examples of performing the actual build step:

    *   If on Windows with Visual Studio, you can open `maxb.sln` from
        within the `build` directory that was created above.

            cmake --open build

    *   To build without an IDE, but strictly on command-line, use the following
        command:

            cmake --build build

    *   The chosen generator used by CMake might support multiple
        configurations. For example, when using Visual Studio, if the
        configuration is omitted, the it will perform a debug build by default.
        In order to change the kind of configuration, please specify it:

            cmake --build build --config Debug
            cmake --build build --config Release
            cmake --build build --config RelWithDebInfo
            cmake --build build --config MinSizeRel


## Usage

At the moment, this application supports testing for UDP bandwidth.

1.  Benchmark how much data can be transferred within a specific time period.
    The time period is specified in seconds. In this example, 10 seconds
    is used:

        maxb 10.28.1.130 10


## License

`maxb` is licensed under version 3 of the GPL or later. See [LICENSE.txt][1] for more info.


[1]: LICENSE.txt
