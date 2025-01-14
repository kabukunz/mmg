echo OFF

@REM clean
del /Q /F client\build
del /Q /F client\bin 

@REM setup
set MMG_SRC=%cd%
set MMG_INSTALL_DIR=%MMG_SRC%\install

@REM build type
if NOT DEFINED "%MMG_BUILD_TYPE%" (
    set MMG_BUILD_TYPE=Release
)

@REM @REM build mmg 
@REM call client\build_mmg2d.bat

cd client

@REM build client
cmake -B build -G "Ninja" ^
-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE ^
-DCMAKE_BUILD_TYPE=%MMG_BUILD_TYPE% ^
-DMMG_SRC=%MMG_SRC% ^
-DMMG_INSTALL=%MMG_INSTALL_DIR%

cmake --build build

@REM test client
bin\mmg2dclient.exe ^
    ..\libexamples\mmg2d\adaptation_example0\example0_a\init.mesh ^
    bin\init_out.mesh

cd ..
