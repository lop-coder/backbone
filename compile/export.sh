#!/bin/bash
######################################################################################
REP_DIR=extension
if [ $# -ge 1 ]; then
    REP_DIR=$1
fi
echo ${REP_DIR}
######################################################################################
cd ${REP_DIR}/build
conan export-pkg ..   -f
version=`cat ../conanfile.py|grep version`
version=${version##*=}
version=${version#*\"}
version=${version%*\"}
echo =====${version}=====

name=`cat ../conanfile.py|grep name`
name=${name##*=}
name=${name#*\"}
name=${name%*\"}
echo =====${name}=====

#conan upload ${name}/${version} -r turbo --all
######################################################################################
