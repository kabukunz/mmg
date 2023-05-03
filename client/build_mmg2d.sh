#!/bin/bash

export MMG_SRC=$(pwd)
export MMG_BUILD="${MMG_SRC}/build"
export MMG_INSTALL="${MMG_SRC}/install"

rm -rf $MMG_BUILD
rm -rf $MMG_INSTALL

cmake -H. -B $MMG_BUILD -G Ninja \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DCMAKE_BUILD_TYPE=Release \
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
-DCMAKE_INSTALL_PREFIX:PATH=$MMG_INSTALL

cmake --build $MMG_BUILD
cmake --build $MMG_BUILD --target install

export LD_LIBRARY_PATH="${MMG_INSTALL}/lib"
cp "${MMG_SRC}/client/square.mesh" "${MMG_INSTALL}/bin"
"${MMG_INSTALL}/bin/mmg2d_O3" "${MMG_INSTALL}/bin/square.mesh" "${MMG_INSTALL}/bin/square_out.mesh"
rm -rf "${MMG_INSTALL}/bin/square*"

