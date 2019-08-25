#!/bin/bash

# Build script for building 'BlockModDemo' and all dependend libraries

# Command line options:
#   [reldeb|release|debug]		build type
#   [2 [1..n]]					cpu count
#   [verbose]					enable cmake to call verbose makefiles

# path export for mac
export PATH=~/Qt/5.11.3/gcc_64/bin:~/Qt/5.11.3/clang_64/bin:$PATH

CMAKELISTSDIR=$(pwd)
BUILDDIR="bb"

# set defaults
CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo"
MAKE_CPUCOUNT="2"
BUILD_DIR_SUFFIX="gcc"
COMPILER=""
SKIP_TESTS="false"

# parse parameters, except gprof and threadchecker
for var in "$@"
do

    if [[ $var = *[[:digit:]]* ]];
    then
		MAKE_CPUCOUNT=$var
		echo "Using $MAKE_CPUCOUNT CPUs for compilation"
    fi

    if [[ $var = "macbrew"  ]];
    then
		CMAKE_OPTIONS="$CMAKE_OPTIONS -DCMAKE_C_COMPILER=/usr/local/Cellar/gcc/8.1.0/bin/gcc-8 -DCMAKE_CXX_COMPILER=/usr/local/Cellar/gcc/8.1.0/bin/g++-8"
		echo "Using gcc from homebrew installation"
    fi
	
    if [[ $var = "debug"  ]];
    then
		CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=Debug"
		echo "Debug build..."
    fi

    if [[ $var = "release"  ]];
    then
		CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=Release"
		echo "Release build..."
    fi

    if [[ $var = "reldeb"  ]];
    then
		CMAKE_BUILD_TYPE=" -DCMAKE_BUILD_TYPE:STRING=RelWithDebInfo"
		echo "RelWithDebInfo build..."
    fi

    if [[ $var = "verbose"  ]];
  	then
		CMAKE_OPTIONS="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"
	  fi

done


# create build dir if not exists
BUILDDIR=$BUILDDIR-$BUILD_DIR_SUFFIX
if [ ! -d $BUILDDIR ]; then
    mkdir -p $BUILDDIR
fi &&

cd $BUILDDIR && cmake $CMAKE_OPTIONS $CMAKE_BUILD_TYPE $CMAKE_COMPILER_OPTIONS $CMAKELISTSDIR && make -j$MAKE_CPUCOUNT &&
cd $CMAKELISTSDIR &&

echo "*** Copying BlockModDemo to bin/release ***" &&
mkdir -p ../../bin/release &&
if [ -d $BUILDDIR/BlockModDemo/BlockModDemo.app ]
then
    rm -rf ../../bin/release/BlockModDemo.app
    cp -r $BUILDDIR/BlockModDemo/BlockModDemo.app ../../bin/release/BlockModDemo.app
else
    cp $BUILDDIR/BlockModDemo/BlockModDemo ../../bin/release/BlockModDemo
fi
