#!/bin/bash

# clean
rm -rf client/build
rm -rf client/bin

# build mmg
client/build_mmg2d.sh

export MMG_SRC=$(pwd)
export MMG_INSTALL_DIR="${MMG_SRC}/install"

if [ -z ${MMG_BUILD_TYPE} ] ; then
    export MMG_BUILD_TYPE="Release"
fi

cd client

# build client
cmake -B build -G "Ninja" \
-DCMAKE_BUILD_TYPE=${MMG_BUILD_TYPE} \
-DMMG_SRC=$MMG_SRC \
-DMMG_INSTALL=$MMG_INSTALL_DIR

cmake --build build

# test client
bin/mmg2dclient \
    ../libexamples/mmg2d/adaptation_example0/example0_a/init.mesh \
    bin/init_out.mesh

cd ..
