#!/bin/bash

set -e

echo --------------------------------------------------
echo TEST CATHLS GROUP : `pwd`
echo --------------------------------------------------

set -v

for i in */test_cathls.sh
do
 (
        cd "$(dirname "$i")" && ./$(basename "$i")
 )
done

echo --------------------------------------------------
echo TEST CATHLS GROUP FINISH: `pwd`
echo --------------------------------------------------
