#!/bin/sh

clear()
{
    rm -rf "$SCRIPTPATH/build" "$SCRIPTPATH/install"
}

SCRIPTPATH="$PWD/$(dirname $0)"
echo "$SCRIPTPATH"

clear
mkdir -p "$SCRIPTPATH/build"
cd "$SCRIPTPATH/build"
cmake "$SCRIPTPATH" -DCMAKE_INSTALL_PREFIX="$SCRIPTPATH/install"
make clean
NUMCORES="$(grep processor /proc/cpuinfo | wc -l)"
make -j"$NUMCORES"
make install
make package

LD_LIBRARY_PATH=../src ./unittest/unittest_rest
if [ "$?" -ne "0" ]; then
	echo "Build failed. Deleting output."
	clear
fi
