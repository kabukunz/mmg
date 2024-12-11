#!/bin/bash

export DIST="mmg-5.7.0-Darwin-19.5.0.zip"

mkdir pack
cd install
7z a $DIST lib include bin share
mv $DIST ../pack
cd ..
