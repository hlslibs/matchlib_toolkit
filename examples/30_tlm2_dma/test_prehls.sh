#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

make sim_sc
./sim_sc

make fast_sim_sc
./fast_sim_sc

make clean
