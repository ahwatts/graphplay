## Graphplay

This is a playground where I'm fooling around with OpenGL 4 and simulation.

### Dependencies

* [glfw](http://www.glfw.org/) 3.1+
* [glm](http://glm.g-truc.net/) 0.9.6+
* [boost](http://www.boost.org/) 1.58+
* An OpenGL driver that supports OpenGL 4.1+.

### Building

There's three basic ways to build it:

1. [CMake](https://cmake.org/)
1. Visual Studio 2015 (on Windows, natch)
1. XCode (on Mac, natch)

#### CMake

From the project root directory:

    $ mkdir build
    $ cd build
    $ cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
    ...
    $ make

On Windows, I use the "MinGW Makefiles" option and then `mingw32-make`.

There's also a `Makefile` in the root directory to run the build from there.

#### Visual Studio

Load up `graphplay.sln` and build it.

#### XCode

Open up `graphplay.xcodeproj` and build it. I don't build with XCode as often as I do CMake or Visual Studio, so it might not be 100% up-to-date.

### Running

After compiling, from the root directory:

    $ ./build/graphplay/graphplay

To run the tests:

    $ ./build/graphplay-test/graphplay-test

On Windows, the `graphplay` executable uses the "Windows" subsystem, so it will not produce any output to the terminal. To help with debugging, there is a `graphplay-console` executable on Windows which runs in a terminal and displays standard output to that shell.
