#!/bin/sh
TOP_DIR=${PWD}
#Input Atlas 500 DDK Path
ATLAS_DDK_PATH=""

#Input Atlas 500 compile path
ATLAS_COMPILE_PATH=""

ATLAS_500_HOST="lib64"

if [ ! -d ${ATLAS_DDK_PATH} ]; then
    echo "ddk path is not exist"
    exit 1
fi

cd ${ATLAS_DDK_PATH}/${ATLAS_500_HOST}/
ln -s libprotobuf.so.15 libprotobuf.so

cd ${TOP_DIR} && make clean; make ddk_path=${ATLAS_DDK_PATH} compile_path=${ATLAS_COMPILE_PATH}

for file in ${TOP_DIR}/*
do
if [ -d "$file" ]
then
  if [ -f "$file/Makefile" ];then
        cd $file && make clean; make ddk_path=${ATLAS_DDK_PATH} compile_path=${ATLAS_COMPILE_PATH}
    if [ -d "$file/tmp" ];then 
        rm -r "$file/tmp"
    fi
  fi
fi
done

if [ ! -d ${TOP_DIR}/out/lib ]; then
    mkdir -p ${TOP_DIR}/out/lib
else
    rm -rf ${TOP_DIR}/out/lib/*
fi

cp -r ${ATLAS_DDK_PATH}/${ATLAS_500_HOST}/* ${TOP_DIR}/out/lib

