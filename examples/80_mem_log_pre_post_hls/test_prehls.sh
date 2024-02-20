#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v

if command -v catapult > /dev/null 2>&1; then
 if [ -z "$MGC_HOME" ]; then
   exit 0
 fi

 make build
 ./sim_sc
 make clean
fi
