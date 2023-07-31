#!/bin/bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cd build
sudo make install
cd ..
rm -rf build