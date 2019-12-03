EN|[CN](README.zh.md)
# VdecPerformance

## Introduction

This sample demonstrates how to test the image decoding & encoding performance of Dvpp module in Ascend310.

Process Framework

    SrcEngine(Device) > JpegdEngine/PngdEngine/JpegeEngine(Device) > DstEngine(Host)

## Supported Products

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Compilation

Compile the Atlas 300 (Model 3000) or Atlas 300 (Model 3010) program:
```bash
./build.sh A300
```

Compile the Atlas 500 (Model 3010) program:
```bash
./build.sh A500
```

Note: the default run parameter of the compile script build.sh is "A300". It will compile Atlas300 program when running following command:
```bash
./build.sh 
```

## Execution

View the help document:
```bash
cd out
./main -h
```
Usage: dvppTest [Options...]

    Options:
    -h                             show usage message.
    -d                             dvpp type:
                                       0  - decode jpeg
                                       1  - decode png
                                       2  - encode jpeg
    -i                             input file path
    -s                             start chip id, default is 0. range[0, 3]
    -e                             end chip id, default is 0. range[0, 3]
    -g                             graph count per chip, default is 1. range[1, 10]
    -t                             thread count per graph, default is 1. range[1, 16]
    -width                         yuv pic width, default is 1. range[32, 8192], only encode jpeg need
    -height                        yuv pic height, default is 1. range[32, 8192], only encode jpeg need
    -format                        yuv pic format:
                                       0  - JPGENC_FORMAT_UYVY
                                       1  - JPGENC_FORMAT_VYUY
                                       2  - JPGENC_FORMAT_YVYU
                                       3  - JPGENC_FORMAT_YUYV
                                       16 - JPGENC_FORMAT_NV12
                                       17 - JPGENC_FORMAT_NV21
    -level                         yuv pic level, default is 1. range[1, 100], only encode jpeg need

    Eg:
    ./main -d 0 -i ../data/test_1080p.jpg -s 0 -e 1 -g 1 -t 8



