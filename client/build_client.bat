
@REM clean
del /Q /F client\build
del /Q /F client\bin 

@REM build mmg 
call client\build_mmg2d.bat

cd client

@REM build client
cmake -B build -G "Ninja" ^
-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE ^
-DCMAKE_BUILD_TYPE=Release

cmake --build build

@REM test client
bin\mmg2dclient.exe ^
    ..\libexamples\mmg2d\adaptation_example0\example0_a\init.mesh ^
    bin\init_out.mesh

cd ..
