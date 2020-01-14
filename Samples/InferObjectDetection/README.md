EN|[CN](README.zh.md)

# SSD ObjectDection Sample


## Introduction

This Sample demonstrate how to implement the following procedure.

    Take H264/JPG as input -> decode the video/image -> data format transformation -> ObjectDetection -> Output the results

These APIs are strongly involved in this Sample: VDEC, VPC, JPEGE, AIModelManager

## Supported Products

Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

You may run the following command to get the currently installed version of Atlas product in the environment:
```bash
npu-smi info
```

## Dependency

1. Single input supported, the ouput layer would be the SSDDetection ObjectDetection model, like VGG-SSD, resnet-SSD etc. Other models may requires different implementation in the code, sample model please refer to <Project Root Folder>/data/README.md.
2. JPEG in Huffman Coding is supproted; But th following codings are not: Arithmetic Coding, Progressive Coding and JPEG 2000.
3. This sample supoorts 264 format video input, you can use ffmpeg to tranform other file into h264 file like the following:
```
    /*-bf number of B Frame, -g internal between key frames, -s resolution, -an close the sound track, -r frame rate setting*/
    ffmpeg -i test.mp4 -vcodec h264 -bf 0 -g 25 -r 10 -s 1280*720 -an -f h264 test1.264
```

## model convert

Download and convert related models by referring to [model transformation instructions](data/models/README.md).

## Compilation

Compile this sample as Atlas 800 (Model 3000) or Atlas 300 (Model 3010) program:
```bash
export DDK_HOME= <Atlas 300 model 300* DDK_HOME>/ddk
cd <Project Root Folder>
bash build.sh A300
```

Compile this sample as Atlas 500 (Model 3010) program:
```bash
export DDK_HOME= <Atlas 500 DDK_HOME>/ddk
cd <Project Root Folder>
bash build.sh A500
```

Note: the default run parameter of the compile script build.sh is "A300". It will compile Atlas300 program when running following command:
```bash
./build.sh
```

## Execution

```bash
cd out
./ObjectDetection -h

ObjectDetection [OPTION]
Options:

    -h                             Print a usage message.
    -i '<path>'                    Optional. Specify the input file: jpg iamge or H264 Video, default: test.jpeg
    -g '<graph file>'              Optional. Specify the graph config file, default: graph.config
    -m '<model file>'              Optional. Specify the model file, default: load from graph.config
    -c '<chanel number in graph>'  Optional. Specify the number of video channle in one graph, range [1, 4], default: 1
    -s '<device id>'               start chip id, default is 0
    -e '<device id>'               end chip id, default is 0

Eg: ./ObjectDetection -i ./test.jpg -m ./ObjectDetection.om -g ./graph.config -s 0 -e 1

```


 
