#!/bin/bash

basename=${1:-"channel_logs"}

if [ ! -d "${basename}" ]; then
  mkdir "${basename}"
fi

while IFS=" " read -r var1 var2 ; do
  echo Processing $var2
  grep "^$var1 " ${basename}_data.txt | sed "s/^$var1 | //" | sed "s/ | .*//" > ${basename}/$var2.txt
  grep "^$var1 " ${basename}_data.txt | sed "s/^.*| //" > ${basename}/$var2.txt.times
done < ${basename}_names.txt

echo $0: Done
