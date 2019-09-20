#!/bin/bash

path_cur=$(cd `dirname $0`; pwd)
build_type="Release"

source $path_cur/../Common/scripts/build_tools.sh

checkEnvs
checkArgs "$@"

if [ $build_target == "A300" ]; then	
	buildA300 $build_target $build_type $path_cur
else
	buildA500 $build_target $build_type $path_cur
fi
