#!/bin/bash

mkdir pack
cd install
7z a mmg-5.7.0-Darwin-19.5.0.zip lib include bin share
mv mmg-5.7.0-Darwin-19.5.0.zip ../pack
cd ..
