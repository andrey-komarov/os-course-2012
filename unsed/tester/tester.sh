#!/bin/bash

pushd ..
make
popd

make

./gen 20 40 | ../readlines-main 10
