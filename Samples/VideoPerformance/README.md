EN|[CN](README.zh.md)
# VideoPerformance

## Introduction

This sample demonstrates how to test the vedio decoding of Vdec module or encoding performance of Venc module in Ascend310.

    Process Framework

    SrcEngine(Device) > VDecEngine(Device)/VencEngine(Device) > DstEngine(Host)

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
## Attention！
In order to test the performance correctly, the video file must contain more than 300 frame images. Additionally, the size of video file should not larger than 128MB.


## Execution

View the help document:
```bash
cd out
./main -h
```
    Usage: main [Options...]

    Options:
    -h                             Print a usage message.
    -s                             Optional. Specify the start No of test chip, default: 0, range = [0, 3]
    -e                             Optional. Specify the end No of test chip, default: 0, range = [0, 3]
    -g                             optional. Specify the numbers of graph per chip, default: 1,     need to less than 3 when encoding video
    -t                             optional. Specify the numbers of thread per graph, default: 1,     range = [1, 16] when -v = 0, range = [1] when -v = 1
    -i '<path>'                    Optional. Specify the input vedio, default: test.h264
    -d                             optional. Specify the type of vedio file when decodingvideo,     0: h264, 1: h265, default: 0
                                   Specify the type of video after encoding yuv images,     0: h265 main level, 1: h264 baseline level , 2: h264 main level, 3: h264 high level, default: 0
    -v                             Optional. Specify the test type, 0: decode video, 1: encode video, default: 0
    -height                        Optional. Specify the height of yuv image when encoding video, default: 1080     range = [128, 1920]
    -width                         Optional. Specify the width of yuv image when encoding video, default: 1920     range = [128, 1920]

Excute the program:

```bash
cd out
./main -i ../test.h264 -s 0 -e 3 -t 4 -g 4
```

