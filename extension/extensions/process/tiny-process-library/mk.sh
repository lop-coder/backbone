#!/bin/bash
set -e
######################################################################################
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
echo -e "Build Path:   "${REP_BUILD_DIR}
echo -e "==========================================="
######################################################################################
cd ${REP_BUILD_DIR}
######################################################################################
rm -rf compile.log
rm -rf error.log
######################################################################################
echo -e ""
echo -e "  Install Dependencies ..."
conan install .. -pr=../${PR} --build=missing  1>>compile.log
######################################################################################
echo -e ""
echo -e "  Generate dependency graph (graph.html) ..."
conan info .. --graph=graph.html 1>>compile.log
######################################################################################
echo -e ""
echo -e "  Build ..."
conan --version 1>>compile.log
conan build .. --configure 1>>compile.log
cmake --build . --config ${BUILD_TYPE} -j 1 1>>compile.log
######################################################################################
echo -e ""
echo -e "  Install ..."
cmake --install . 1>>compile.log
######################################################################################
cd -
######################################################################################
echo -e ""
echo -e "  Export ..."
conan export-pkg .  -f
######################################################################################

