#!/bin/sh

rm -rf "$(dirname $0)/build" "$(dirname $0)/install"
mkdir -p "$(dirname $0)/build"
cd "$(dirname $0)/build"
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make clean
NUMCORES="$(grep processor /proc/cpuinfo | wc -l)"
make -j"$NUMCORES"
make install
make package
