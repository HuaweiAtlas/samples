EN|[CN](README.zh.md)
# DecodeImage

## Introduction

This sample demonstrates how to use the DecodeImage program to decode JPEG or PNG images in YUV format, resize the images, and save the images to the local host.
Process Framework

    Jpeg Decode Resize: MindInput(Host) > JPEGDResize(Device) > SaveFile(Host)
    Png  Decode Resize: MindInput(Host) > PNGDResize(Device)  > SaveFile(Host)

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
    -h         Print a usage message.
    -i         Optional. Specify the image path, a directory or file. Default: ../data/test.jpeg.
    -r         Optional. Input path type. Default: 0.
                    0 - Input path is a file.
                    1 - Input path is a directory.
    -d         Optional. Specify the dvpp type. Default: -1.
                    0 - decode JPEG
                    1 - decode PNG
               Jpeg resolutions: maximum 4096 x 4096, minimum 32 x 32.
               Png  resolutions: maximum 4096 x 4096, minimum 32 x 32.
                    
Decode and resize all JPEG images in the ../data folder:
```bash
cd out
./main -i ../data/ -r 1 -d 0
```

## Constraint
```
Jpeg resolutions: maximum 4096 x 4096, minimum 32 x 32.
Png  resolutions: maximum 4096 x 4096, minimum 32 x 32.
```

## Result
```
The yuv file is generated in the out folder out/result_files.
```






