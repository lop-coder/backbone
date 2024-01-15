#!/usr/bin/bash

set -e

cd `dirname $0`

cmd='cpplint --filter=-build,-legal --recursive --extensions=h,hpp,c,cpp --quiet'

while read line; do
  if [[ "$line" != "" ]]; then
    cmd="${cmd} --exclude=${line}"
  fi
done < .ignorecheck

cmd="${cmd} ."

${cmd}

