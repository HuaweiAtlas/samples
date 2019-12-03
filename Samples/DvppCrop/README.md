EN|[CN](README.zh.md)
## Introduction

This sample demonstrates how to use the DvppCrop program to decode JPEG images in YUV format, cut out the images, and save the images to the local host.
Process Framework

    DvppCrop: main(Host) > DvppCrop(Device) > SaveFile(Host)

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
    -h                         Print a usage message.
    -i  '<path>'               Optional. Specify an image. This sample only supports the jpeg format.
                               Default: ../data/test.jpeg. 
                               Resolutions constraint: maximum 8192 x 8192, minimum 32 x 32.
Decode and cut out the ../data/test.jpeg image:
```bash
cd out
./main -i ../data/test.jpeg
```

## Constraint

resolutions: maximum 4096 x 4096, minimum 32 x 32.


## Result

The res.yuv file is generated in the out folder.


