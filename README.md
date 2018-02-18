# Concurrent Data Structures

[![Build Status](https://travis-ci.org/Hoshiningen/concurrent-data-structures.svg?branch=master)](https://travis-ci.org/Hoshiningen/concurrent-data-structures)

This is a set of concurrent data structures whose purpose is to explore the performance and scalability of their respective design techniques. They are minimal implementations for the sake of the comparative analysis, and there are no plans to fully implement robust versions of each.

## Building

Make sure you have `git` and at least `Cmake 3.9.2` installed on your system.

To build the project, it's necessary to build it's dependencies first if they aren't present on your system. Execute the following in a directory you wish to build the dependencies in:

```shel
./build_support/install_gbench.sh
```

This will clone Google benchmark and its dependencies, and will by default install them in `/usr/local` on linux. This is configurable by changing the `CMAKE_INSTALL_PREFIX` to a more desirable destination.

Next, to build the project, execute:

```
mkdir build && cd build
cmake ..
```

If you're on linux, then you can build the project with:

```
make
```

On Windows, the project can be built through Visual Studio by opening the project as a Cmake project.

## Compiler Support

The project and its dependencies use C++11, so please use a toolchain that supports it. The following are all minimum versions that can be used to build this project and its dependencies:

* GCC 4.8
* Clang 3.4
* Visual Studio 2013