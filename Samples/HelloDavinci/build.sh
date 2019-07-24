#!/bin/bash

if [ -z $DDK_HOME ];then
	echo "[ERROR] DDK_HOME does not exist! Please set environment variable: export DDK_HOME=<root folder of ddk>"
	echo "eg:  export DDK_HOME=/home/<ddk install user>/tools/che/ddk/ddk/"
	exit 0
fi

path_cur=$(cd `dirname $0`; pwd)
path_root=$(cd $path_cur/../..; pwd)
path_cmake_module=$path_root/Cmake
path_arm_toolchain=$path_cmake_module/aarch64.cmake
path_build=$path_cur/build
path_out=$path_cur/out
build_type="Release" #Release|Debug

function build_device()
{
    mkdir -p $path_build
    cd $path_build
    cmake -DBUILD_TARGET=DEVICE \
          -DCMAKE_BUILD_TYPE=$build_type \
          -DCMAKE_MODULE_PATH=$path_cmake_module \
          -DCMAKE_TOOLCHAIN_FILE=$path_arm_toolchain \
          $path_cur
    make
    cp *.so $path_out
    rm -rf $path_build
}

function build_host_x64()
{
    mkdir -p $path_build
    cd $path_build
    cmake -DBUILD_TARGET=HOST \
          -DCMAKE_BUILD_TYPE=$build_type \
          -DCMAKE_MODULE_PATH=$path_cmake_module \
          $path_cur
    make
    cp main $path_out
    rm -rf $path_build
}

function build_host_aarch64()
{
    mkdir -p $path_build
    cd $path_build
    cmake -DBUILD_TARGET=HOST \
          -DCMAKE_BUILD_TYPE=$build_type \
          -DCMAKE_MODULE_PATH=$path_cmake_module \
          -DCMAKE_TOOLCHAIN_FILE=$path_arm_toolchain \
          $path_cur
    make
    cp main $path_out
    rm -rf $path_build
}

function clear_out()
{
    rm -rf $path_out
    mkdir -p $path_out
}

function copy_res()
{
    cp $path_cur/graph.config $path_out
    mkdir -p $path_out/result_files
}

function build_x64()
{
    clear_out
    build_device
    build_host_x64
    copy_res
}

build_x64
