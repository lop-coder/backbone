#!/usr/bin/bash

set -e

cd `dirname $0`

# 请确保camke能通过 [暂无cmake]
# mkdir -p build && cd build && cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cd ..

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

files=`find ./ -name '*.h' -o -name '*.hpp' -o -name '*.c' -o -name '*.cpp' | grep -Ev $exclude`

clang-tidy -p build $files

