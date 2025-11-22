#!/bin/bash
PROJDIR=$PWD
TOOLCHAIN=CMAKE_TOOLCHAIN_FILE=~/Arduino-CMake-Toolchain/Arduino-toolchain.cmake
cd ../..
mkdir -p Arduino4Eclipse
cd Arduino4Eclipse
mkdir core
touch core/build.opt
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=MinSizeRel -D $TOOLCHAIN $PROJDIR
echo "adapt the BoardOptions.cmake file"
