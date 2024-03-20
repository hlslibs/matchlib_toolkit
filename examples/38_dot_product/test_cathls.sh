#!/bin/bash

set -e

echo --------------------------------------------------
echo CATHLS TEST: `pwd`
echo --------------------------------------------------

set -v

if command -v catapult > /dev/null 2>&1; then
  catapult -shell -f go_hls1.tcl
fi

make clean

if command -v catapult > /dev/null 2>&1; then
  catapult -shell -f go_hls2.tcl
fi

make clean

if command -v catapult > /dev/null 2>&1; then
  catapult -shell -f go_hls3.tcl
fi

make clean
