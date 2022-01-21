#!/bin/bash

log_dir=${1:-"log_dir"}

log_dir2=${2:-"log_dir2"}

if [[ "$log_dir2" = /* ]]; then
  new_log_dir2=$log_dir2
else
  new_log_dir2=../$log_dir2
fi

FOUND_TXT=0


cd $log_dir

rm -f earliest.list

for i in *
do
  if [[ $i == *"log_data.txt"* ]]; then
    continue
  fi

  if [[ $i == *"log_names.txt"* ]]; then
    continue
  fi

  if [[ $i == *"log_times.txt"* ]]; then
    continue
  fi

  if [[ $i != *".txt" ]]; then
    continue
  fi

  FOUND_TXT=1

  cmp $i $new_log_dir2/$i > $i.cmp
  if [ $? != 0 ]; then
    diff $i $new_log_dir2/$i > $i.diff
    LINE=$(cat $i.cmp | sed "s/.* line //")
    tail -n +$LINE $i.times > $i.tail
    while IFS=" " read -r t1 units ; do
      break
    done < $i.tail
    rm $i.tail
    tail -n +$LINE $new_log_dir2/$i.times > $i.tail
    while IFS=" " read -r t2 units ; do
      break
    done < $i.tail
    rm $i.tail
    echo $t1 $t2 $i >> earliest.list
  fi

  rm $i.cmp
done

if [ $FOUND_TXT != 1 ]; then
  echo No ".txt" files found in $log_dir . Make sure to run gen_logs.sh
  exit 1
fi

if [ -f earliest.list ]; then
  sort -n earliest.list > earliest.sorted
  echo
  echo -----------------
  echo First 10 channels with differences, format is:
  echo $log_dir time "|" $log_dir2 time "|" file_name 
  echo
  head -10 earliest.sorted
  echo
  echo -----------------

  while IFS=" " read -r t1 t2 fname ; do
    echo
    echo Earliest differences :
    echo "<" is $log_dir/$fname
    echo ">" is $log_dir2/$fname
    echo
    diff $fname $new_log_dir2/$fname | head -30
    echo -----------------
    echo
    break
  done < earliest.sorted
  exit 0
else
   echo No differences in logs detected.
   exit 0
fi
