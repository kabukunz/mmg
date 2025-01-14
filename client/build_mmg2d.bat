echo OFF

@REM setup
set MMG_SRC=%cd%
set MMG_BUILD_DIR=%MMG_SRC%\build
set MMG_INSTALL_DIR=%MMG_SRC%\install

@REM build type
if NOT DEFINED "%MMG_BUILD_TYPE%" (
    set MMG_BUILD_TYPE=Release
)

if "%MMG_BUILD_TYPE%" == "Release" (
    set MMG_BUILD_EXE=mmg2d_O3
) 

if "%MMG_BUILD_TYPE%" == "Debug" (
    set MMG_BUILD_EXE=mmg2d_debug
) 

@REM clean
del /Q /F %MMG_BUILD_DIR% 
del /Q /F %MMG_INSTALL_DIR%

@REM build
cmake -B %MMG_BUILD_DIR% -G Ninja ^
-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE ^
-DCMAKE_BUILD_TYPE=%MMG_BUILD_TYPE% ^
 ^
-DBUILD=MMG2D ^
-DBUILD_TESTING=OFF ^
-DTEST_LIBMMG2D=OFF ^
-DBUILD_SHARED_LIBS=ON ^
-DLIBMMG2D_STATIC=OFF ^
-DLIBMMG2D_SHARED=ON ^
-DUSE_SCOTCH=OFF ^
-DUSE_ELAS=OFF ^
-DUSE_VTK=OFF ^
-DCMAKE_INSTALL_PREFIX:PATH=%MMG_INSTALL_DIR%

@REM compile
cmake --build %MMG_BUILD_DIR%
cmake --build %MMG_BUILD_DIR% --target install

@REM test
copy %MMG_SRC%\client\square.mesh %MMG_INSTALL_DIR%\bin
%MMG_INSTALL_DIR%\bin\mmg2d_O3 %MMG_INSTALL_DIR%\bin\square.mesh %MMG_INSTALL_DIR%\bin\square_out.mesh
del /Q /F %MMG_INSTALL_DIR%\bin\square*.* 
