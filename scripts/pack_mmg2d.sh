#!/bin/bash

cd build/Release
mkdir pack
cd install
7z a mmg-5.5.2-Darwin-19.5.0.zip lib include
mv mmg-5.5.2-Darwin-19.5.0.zip ../pack
cd ../..