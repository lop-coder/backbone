#!/bin/bash
if [ $# -ne 1 ]; then
	echo ""
	echo $#
	echo "=========Error,The version must be specified, like this: scripts/version.sh "1.2.3"========"
	exit -1
fi

if [ ! -f src/conanfile.py ];then
	echo ""
	echo "=========Error,the file src/conanfile.py does not exist========"
	exit -1
fi
sed 's/version.*/version=\"'$1'\"/'  -i src/conanfile.py

