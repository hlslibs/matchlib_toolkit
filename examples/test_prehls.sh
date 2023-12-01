#!/bin/bash

set -e

echo --------------------------------------------------
echo TEST PREHLS GROUP START: `pwd`
echo --------------------------------------------------

set -v

for i in */test_prehls.sh
do
 (
        cd "$(dirname "$i")" && ./$(basename "$i")
 )
done

echo --------------------------------------------------
echo TEST PREHLS GROUP FINISH: `pwd`
echo --------------------------------------------------
