#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

make marshall_port_sc
./marshall_port_sc

make direct_port_sc
./direct_port_sc

make fast_sim_sc
./fast_sim_sc

if command -v clang++ > /dev/null 2>&1; then
  make clang_sc
  ./clang_sc
fi

make clean
