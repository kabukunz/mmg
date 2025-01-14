echo OFF

set DIST=mmg-5.8.0-Windows-10.0.19043.zip

mkdir pack
cd install
"C:\Program Files\7-Zip\7z.exe" a %DIST% lib include bin share
move %DIST% ../pack
cd ..
