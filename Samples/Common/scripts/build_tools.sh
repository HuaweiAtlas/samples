#!/bin/bash

function checkEnvs()
{
    if [ -z $DDK_HOME ];then
	    echo "[ERROR] DDK_HOME does not exist! Please set environment variable: export DDK_HOME=<root folder of ddk>"
	    echo "eg:  export DDK_HOME==/home/<ddk install user>/tools/che/ddk/ddk/"
	    exit 0
    fi
}

function checkArgs()
{
   if [ $# -eq 0 ]; then
       build_target='A300'
       return 0
   fi 
   if [ $# -ne 1 ]; then
       echo "[error] parameter number should be equal to 1 , input parameter should be \"A300\" or \"A500\" to run build.sh."
       exit -1
   fi
   if [ "$1" != "A300" -a "$1" != "A500" ]; then
       echo "The input parameter should be \"A300\" or \"A500\" to run build.sh."
       exit -1
   fi  
   build_target=$1
}

function preparePath() {
    rm -rf $1
    mkdir -p  $1
    cd  $1
}

function buildA300() {
    path_build=$3/out
    path_cmake=$3/build
    preparePath $path_build
    cmake -DCMAKE_BUILD_TARGET=$1 \
	      -DCMAKE_BUILD_TYPE=$2 $path_cmake
    make -j8
}

function buildA500() {
    path_build=$3/out
    path_cmake=$3/build
    path_toolchain=$3/../CMake/Euler.cmake
    preparePath $path_build
    cmake -DCMAKE_BUILD_TARGET=$1 \
	      -DCMAKE_BUILD_TYPE=$2 \
		  -DCMAKE_TOOLCHAIN_FILE=$path_toolchain $path_cmake
    make -j8
}
