echo OFF

@REM uncomment for debug
@REM set MMG_BUILD_TYPE=Debug

call client\build_mmg2d.bat
call client\build_client.bat
call client\pack_mmg2d.bat
