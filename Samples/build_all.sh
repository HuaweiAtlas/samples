#/bin/bash

CurrentFolder="$( cd "$(dirname "$0")" ; pwd -P )"
source $CurrentFolder/Common/scripts/build_tools.sh

checkEnvs
checkArgs "$@"

SampleFolder=(
  HelloDavinci
  InferOfflineVideo
  HaSample
  DynamicGraph
  DvppCrop
  DecodeVideo
  InferObjectDetection
  DecodeImage
  EncodeJpeg
  VideoPerformance
  InferClassification
  DvppImagePerformance
  EncodeVideo
  LogSample)

#compile the sample
for SamplePtr in ${SampleFolder[@]};do
  cd ${CurrentFolder}/${SamplePtr}
  echo -e building "\033[0;31m$PWD\033[0m"
  sed -i 's/\r$//g' build.sh
  chmod +x ./build.sh
  ./build.sh $build_target
done

