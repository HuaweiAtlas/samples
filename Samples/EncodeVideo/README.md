EN|[CN](README.zh.md)
## Introduction

This sample demonstrates how to use the EncodeVideo program to encode a YUV file into a .bin file.
Process Framework

    EncodeVideo: main(Host) > VideoIn(Host) > VideoEncode(Device) > VideoOut(Host)

## Supported Products

Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Compilation

Compile the Atlas 800 (Model 3000) or Atlas 300 (Model 3010) program:
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
    -h                             Print a usage message.
    -i '<path>'                    Requested. Specify the input image, eg, /home/data/test.yuv
    -height                        Requested. Specify the height of input image. range = [128, 1920]
    -width                         Requested. Specify the width of input image.  range = [128, 1920]

```bash
cd out
./main -i=/home/data/test.yuv -height 960 -width 1280 
```
