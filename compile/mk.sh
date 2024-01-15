#!/bin/bash

set -e
######################################################################################
REP=backbone
REP_BUILD_DIR=build
######################################################################################
SYSTEM_TYPE=windows
BUILD_TYPE=Release
ARCH_TYPE=x86_64
######################################################################################
if [ $# -ge 1 ]; then
	if [[  $1 = "linux" ||  $1 = "Linux" ]]; then
		SYSTEM_TYPE=linux
	else
		SYSTEM_TYPE=windows
	fi
fi
if [ $# -ge 2 ]; then
	if [[  $2 = "debug" ||  $2 = "Debug" ]]; then
		BUILD_TYPE=Debug
	else
		BUILD_TYPE=Release
	fi
fi
if [ $# -ge 3 ]; then
	ARCH_TYPE=$3
fi
if [ $# -ge 4 ]; then
	REP=$4
fi
#if [ $# -ge 5 ]; then
	#REP_BUILD_DIR=$5
#fi
######################################################################################
if [[  ${SYSTEM_TYPE} = "linux" ]]; then
	if [[  ${BUILD_TYPE} = "Release" ]]; then
		PR=linux_${ARCH_TYPE}_release
	else
		PR=linux_${ARCH_TYPE}_debug
	fi
else
	if [[  ${BUILD_TYPE} = "Release" ]]; then
		PR=win_${ARCH_TYPE}_release
	else
		PR=win_${ARCH_TYPE}_debug
	fi
fi
######################################################################################
if [ ! -d ${REP_BUILD_DIR} ]; then
	mkdir ${REP_BUILD_DIR}
fi
######################################################################################
echo -e "==========================================="
echo -e "Build Type:   "${BUILD_TYPE}
echo -e "System OS :   "${SYSTEM_TYPE}
echo -e "Arch      :   "${ARCH_TYPE}
echo -e "Rep       :   "${REP}
echo -e "Build Path:   "${REP_BUILD_DIR}
echo -e "==========================================="
######################################################################################
cd ${REP_BUILD_DIR}
######################################################################################
if [[  ${SYSTEM_TYPE} = "linux" ]]; then
	if [ -f bin/${REP} ];then
		rm -rf bin/${REP}
	fi
	if [ -f lib/lib${REP}.so ];then
		rm -rf bin/${REP}.so
	fi
	rm -rf bin/Test
else
	if [ -f bin/${REP}.dll ];then
		rm -rf bin/${REP}.dll
	fi
	if [ -f bin/${REP}.exe ];then
		rm -rf bin/${REP}.exe
	fi
	rm -rf bin/Test.exe
fi
######################################################################################
rm -rf compile.log
rm -rf error.log
######################################################################################
echo -e ""
echo -e "  Install Dependencies ..."
conan install .. -pr=../conan/${PR} --build=missing  1>>compile.log 
######################################################################################
if [ -d install ]; then
   rm -rf install
fi
######################################################################################
echo -e ""
echo -e "  Generate dependency graph (graph.html) ..."
conan info .. --graph=graph.html 1>>compile.log 
######################################################################################
echo -e ""
echo -e "  Build ..."
conan --version 1>>compile.log 
conan build .. --configure 1>>compile.log 
#cmake --build . --config ${BUILD_TYPE} -j 8
cmake --build . --config ${BUILD_TYPE} -j 1 1>>compile.log 
######################################################################################
echo -e ""
echo -e "  Install ..."
cmake --install . 1>>compile.log  
######################################################################################
echo -e ""
echo -e "  Check target generation ..."
if [[  ${SYSTEM_TYPE} = "linux" ]]; then
	if [[ ! -f lib/lib${REP}.so && ! -f bin/${REP} ]];then
		echo -e ""
		echo -e "=========Error,the file lib${REP}.so or ${REP} does not exist========"
		exit -1
	fi
else
	if [[ ! -f bin/${REP}.dll && ! -f bin/${REP}.exe ]];then
		echo -e ""
		echo -e "=========Error,the file ${REP}.dll or ${REP}.exe does not exist========"
		exit -1
	fi
fi
######################################################################################
#echo ""
#echo "  Check the compilation log ..."
#if [[  -f error.log ]];then
#	cat error.txt | while read line
#	do
#	    echo $line
#	done
#fi
#echo "  Check the compilation log end ..."
######################################################################################
echo -e ""
echo -e "  Testing begin ..."
if [[  ${SYSTEM_TYPE} = "linux" ]]; then
	if [ -f bin/Test ];then
		cd lib
		../bin/Test
		cd ..
	else
		echo -e "=========Error bin/Test does not exist========"
	fi
	echo -e ""
	echo -e "  Extension Test ... "
	for file in bin/*
	do
		if [ -x $file ]
		then
			if [[ $file != bin/Test && $file != bin/echoWorker ]]
			then
				echo "  "$file
				cd lib
				../$file
				cd ..
			fi
		fi
	done
else
	if [ -f bin/Test.exe ];then
		cd bin
		./Test.exe
		cd ..
	else
		echo -e "=========Error bin/Test.exe does not exist========"
	fi
	echo -e ""
	echo -e "  Extension Test ... "
	for file in bin/*.exe
	do
		if [ -x $file ]
		then
			if [[ $file != bin/Test.exe && $file != bin/echoWorker.exe ]]
			then
				echo "  "$file
				filename=${file##*/}
				cd bin
				./$filename
				cd ..
			fi
		fi
	done

fi
echo -e "  Testing end ..."
######################################################################################
