#!/bin/bash

path_cur=$(cd `dirname $0`; pwd)
build_type="Release"

source $path_cur/../Common/scripts/build_tools.sh

checkEnvs
checkArgsA35 "$@"

if [ $build_target == "A300" ]; then	
	buildA300 $build_target $build_type $path_cur
elif [ $build_target == "A200" ]; then	
	buildA200 $build_target $build_type $path_cur
else
	buildA500 $build_target $build_type $path_cur
fi
