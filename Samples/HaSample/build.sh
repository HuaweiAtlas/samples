#!/bin/bash

if [ "$1" == "A500" ];then
  rm -rf out
  mkdir out
  cd out
  cmake ..
  make
fi
