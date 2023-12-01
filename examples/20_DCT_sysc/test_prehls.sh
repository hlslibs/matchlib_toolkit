#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

make marshall_port_sc
time ./marshall_port_sc sample.bmp

make direct_port_sc
time ./direct_port_sc sample.bmp

make fast_sim_sc
time ./fast_sim_sc sample.bmp

make clean
