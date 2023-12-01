#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

if command -v catapult > /dev/null 2>&1; then
  make build
  ./sim_sc
  make clean
fi
