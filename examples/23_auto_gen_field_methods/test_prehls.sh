#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

make build
./sim_sc

if command -v clang++ > /dev/null 2>&1; then
  make clang_sc
  ./clang_sc
fi


make clean
