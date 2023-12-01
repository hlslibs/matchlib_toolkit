#!/bin/bash

set -e

echo --------------------------------------------------
echo PREHLS TEST: `pwd`
echo --------------------------------------------------

set -v


if command -v vsim > /dev/null 2>&1; then
  make build
  make clean
fi

