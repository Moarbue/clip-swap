#! /bin/bash

set -xe

CC=${CC:-gcc}
LIBS=-lclipboard
EXE=${EXE:-clip-swap}
TEMP_DIR=tmp

# check if os is linux and add dependencies
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    LIBS="$LIBS -lpthread -lxcb"
fi

# remove possible leftover directory
rm -rf $TEMP_DIR
# create temporary directory
mkdir -p $TEMP_DIR
# enter temporary directory
cd $TEMP_DIR

# clone clipboard library
git clone https://github.com/jtanx/libclipboard.git
# enter directory
cd libclipboard
# build clipboard library
cmake .
make -j4

# leave temporary directory
cd ../..

# build the binary
$CC -Wall -Wextra -O2 main.c -o $EXE -I ./tmp/libclipboard/include -L ./tmp/libclipboard/lib $LIBS

# remove temporary directory
rm -rf $TEMP_DIR