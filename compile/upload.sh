#!/bin/bash

######################################################################################
REP=backbone
#PR=win_release
######################################################################################
if [ $# -eq 1 ]; then
	REP=$1
fi
######################################################################################
REP_DIR=build
echo ${REP_DIR}

######################################################################################
if [[ ! -f build/bin/${REP}.dll && ! -f build/bin/${REP}.exe ]];then
	echo ""
	echo "=========Error,the file ${REP}.dll or ${REP}.exe does not exist========"
	exit -1
fi
######################################################################################
cd ${REP_DIR}
#conan export-pkg ..  -pr=../../${PR}  -f
conan export-pkg ..   -f
version=`cat ../conanfile.py|grep version|cut -d= -f2|sed 's/[[:space:]]//g'|sed 's/^.//'|sed 's/.$//'`
array=(${version//,/ })
for var in ${array[@]}
do
   echo $var
   version=$var
   break
done
echo =====${version}=====

name=`cat ../conanfile.py|grep name`
name=${name##*=}
name=${name#*\"}
name=${name%*\"}
echo =====${name}=====

#conan upload ${name}/${version} -r engine --all    --confirm
######################################################################################
