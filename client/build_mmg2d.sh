#!/bin/bash

# setup
export MMG_SRC=$(pwd)
export MMG_BUILD_DIR="${MMG_SRC}/build"
export MMG_INSTALL_DIR="${MMG_SRC}/install"

# build type
if [ -z ${MMG_BUILD_TYPE} ] ; then
    export MMG_BUILD_TYPE="Release"
fi

if [[ "$MMG_BUILD_TYPE" == "Release" ]]; then
    export MMG_BUILD_EXE="mmg2d_O3"
fi

if [[ "$CMAKE_BUILD_TYPE" == "Debug" ]]; then
    export MMG_BUILD_EXE="mmg2d_debug"
fi

# clean
rm -rf $MMG_BUILD_DIR
rm -rf $MMG_INSTALL_DIR

# build
cmake -H. -B $MMG_BUILD_DIR -G Ninja \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DCMAKE_BUILD_TYPE=${MMG_BUILD_TYPE} \
\
-DBUILD=MMG2D \
-DBUILD_TESTING=OFF \
-DTEST_LIBMMG2D=OFF \
-DBUILD_SHARED_LIBS=ON \
-DLIBMMG2D_STATIC=OFF \
-DLIBMMG2D_SHARED=ON \
-DUSE_SCOTCH=OFF \
-DUSE_ELAS=OFF \
-DUSE_VTK=OFF \
-DCMAKE_INSTALL_PREFIX:PATH=$MMG_INSTALL_DIR

# compile
cmake --build $MMG_BUILD_DIR
cmake --build $MMG_BUILD_DIR --target install

# test
export LD_LIBRARY_PATH="${MMG_INSTALL_DIR}/lib"
cp "${MMG_SRC}/client/square.mesh" "${MMG_INSTALL_DIR}/bin"
"${MMG_INSTALL_DIR}/bin/${MMG_BUILD_EXE}" "${MMG_INSTALL_DIR}/bin/square.mesh" "${MMG_INSTALL_DIR}/bin/square_out.mesh"
rm -rf "${MMG_INSTALL_DIR}/bin/square*"

