#!/bin/bash

rm -rf build
rm -rf bin

./scripts/build_mmg2d.sh
./scripts/compile_mmg2d.sh

cmake -B build -G "Ninja" \
-DCMAKE_BUILD_TYPE=Release

cmake --build build

# test mmg2dclient
./bin/mmg2dclient libexamples/mmg2d/adaptation_example0/example0_a/init.mesh init_out.mesh
