#!/usr/bin/bash

set -e

cd `dirname $0`
mode=$1
exclude=''
while read line; do
  if [[ "$line" != "" ]]; then
    line=`echo $line |  tr -d "\n" |  tr -d "\r"`
    if [[ "$exclude" != "" ]]; then
      exclude="$exclude|"
    fi
    exclude="${exclude}${line}"
  fi
done < .ignorecheck
echo exclude $exclude 
files=`find ../ -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cpp' -o -name '*.hh' | grep -Ev $exclude`
if [[ "$mode" == "check" ]]; then
  clang-format --Werror --dry-run $files
else
  clang-format -i $files
fi
