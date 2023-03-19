
rmdir /S /Q .\build

cmake -B build -G "Ninja" ^
-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE ^
-DCMAKE_BUILD_TYPE=Release ^
 ^
-DBUILD=MMG2D ^
-DBUILD_SHARED_LIBS=ON ^
-DLIBMMG2D_STATIC=OFF ^
-DLIBMMG2D_SHARED=ON ^
-DUSE_SCOTCH=OFF ^
-DUSE_ELAS=OFF ^
-DUSE_VTK=OFF ^
-DBUILD_TESTING=OFF ^
-DTEST_LIBMMG2D=OFF ^
-DCMAKE_INSTALL_PREFIX:PATH=install

cmake --build build
cmake --build build --target install

.\build\install\bin\mmg2d_O3.exe .\libexamples\mmg2d\adaptation_example0\example0_a\init.mesh
