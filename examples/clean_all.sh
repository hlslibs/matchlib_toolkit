#! /bin/sh

for i in */Makefile
do
  j=`dirname $i`
  (cd $j; make clean)
done

