#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

make all
./single_process
./multi_process
./banked_multi_process


make clean

