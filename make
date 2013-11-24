#!/bin/sh

clear()
{
    rm -rf "$SCRIPTPATH/build" "$SCRIPTPATH/install"
}

SCRIPTPATH="$(readlink -f $(dirname $0))"
clear
mkdir -p "$SCRIPTPATH/build"
cd "$SCRIPTPATH/build"
cmake "$SCRIPTPATH" -DCMAKE_INSTALL_PREFIX="$SCRIPTPATH/install"
make clean
make -j"$(grep processor /proc/cpuinfo | wc -l)"
make install
make package

LD_LIBRARY_PATH=../src ./unittest/unittest_rest
if [ "$?" -ne "0" ]; then
	echo "Build failed. Deleting output."
	clear
fi
