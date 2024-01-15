#!/usr/bin/bash

set -e

cd `dirname $0`

exclude=''
while read line; do
  if [[ "$line" != "" ]]; then
    exclude="${exclude} *${line}*"
  fi
done < .ignorecheck
echo exclude $exclude

# 请确保cmake能通过
mkdir -p build && cd build && cmake -Dtest=ON .. -DCMAKE_CXX_FLAGS="-fprofile-arcs -ftest-coverage"
make -j

# 执行测试
./project1
./runUnitTests

lcov -c -d . -o cover.info
lcov --remove cover.info '*/usr/include/*' '*/usr/lib/*' '*/usr/lib64/*' '*/usr/local/include/*' '*/usr/local/lib/*' '*/usr/local/lib64/*' ${exclude} -o final.info

ret=`genhtml -o cover_report --legend final.info | grep 'lines......' | awk '{print $2}' | awk -F'%' '{if ($1 ~ /^[0-9]+\.[0-9]$/ && $1 > 30.0) print 0; else print 1}'`

echo ret = $ret
exit $ret
