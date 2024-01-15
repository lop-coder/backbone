#!/bin/bash
######################################################################################
REP_DIR=build
echo ${REP_DIR}
######################################################################################
cd ${REP_DIR}
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
