#!/bin/bash

# Create a directory to hold Google benchmark
mkdir ThirdParty && cd ThirdParty

# Clone the source and dependencies
git clone https://github.com/google/benchmark.git
git clone https://github.com/google/googletest.git benchmark/googletest

# Build and install globally
mkdir build && cd build
cmake ../benchmark
make
sudo make install && cd \