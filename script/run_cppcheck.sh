#!/usr/bin/bash

set -e

cd `dirname $0`

#cmd='cppcheck --platform=win64 -q --error-exitcode=1 --suppress=missingInclude --enable=warning --enable=information ..'
cmd='cppcheck --platform=win64 -q --error-exitcode=1 --suppress=missingInclude --enable=all ..'
#cmd='cppcheck --platform=win64 --enable=warning --enable=information .'

while read line; do
  if [[ "$line" != "" ]]; then
    line=`echo $line |  tr -d "\n" |  tr -d "\r"`
    cmd="${cmd} -i${line}"
  fi
done < .ignorecheck
echo ${cmd}
${cmd}
