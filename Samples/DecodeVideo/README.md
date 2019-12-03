EN|[CN](README.zh.md)
# DecodeVideo

## Introduction

This sample demonstrates the DecodeVideo program and uses the built-in hard decoding function of the chip to decode videos in the H264 and H265 formats.

```
ScrEngine(Host) > VDecEngine(Device) > DstEngine(Host)
```

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

```bash
cd out
./main -h

Usage: main [Options...]

Options:
    -h                             show usage message.
    -i                             input file path
    -d                             dvpp type:
                                       0 - decode h264
                                       1 - decode h265
                                       default is 0 (h264)
    -g                             graph count per chip, default is 1
Eg:
    ./main -i /path/to/video/file.264

./main -i /path/to/video/file.264
```

## Note

You need to convert the video files into H264 or H265 format. This sample only supports h264 and h265 video, please do not use file of other format as an input, if necessary, please modify the program code.
