#!/bin/sh -e

clear()
{
    rm -rf "$SCRIPTPATH/build" "$SCRIPTPATH/install" "$SCRIPTPATH/doc/html"
}

uncrustifyFiles()
{
    for FILE in $(find ./src ./unittest -name *.cpp -o -name *.hpp); do
        uncrustify --replace --no-backup -c ./uncrustify.cfg -l CPP $FILE
    done
}

SCRIPTPATH="$(readlink -f $(dirname $0))"
clear

cd "$SCRIPTPATH"
uncrustifyFiles

mkdir -p "$SCRIPTPATH/build"
cd "$SCRIPTPATH/build"
cmake "$SCRIPTPATH" -DCMAKE_INSTALL_PREFIX="$SCRIPTPATH/install"
make clean
make check
make -j"$(grep processor /proc/cpuinfo | wc -l)"
make install
make package
make doc

LD_LIBRARY_PATH=../src ./unittest/unittest_rest
if [ "$?" -ne "0" ]; then
	echo "Build failed. Deleting output."
	clear
	exit 1
fi
