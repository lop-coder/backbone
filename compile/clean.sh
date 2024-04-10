#!/bin/bash
REP=extension
if [ $# -ge 1 ]; then
    REP=$1
fi
rm -rf  output
rm -rf ${REP}/build
rm -rf ${REP}/app/build
rm -rf ${REP}/output
rm -rf ${REP}/docs/api
rm -rf ${REP}/CMakeLists.txt.user
if [ ${REP} == "extension"  ]; then
	echo "==========================="
	rm -rf ${REP}/docs/static
fi
