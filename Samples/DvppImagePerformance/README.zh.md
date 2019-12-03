中文|[英文](README.md)
# DvppImagePerformance

## 介绍

本开发样例演示如何测试Ascend310芯片上的Dvpp模块的图片编解码性能.

流程框架

    SrcEngine(Device) > JpegdEngine/PngdEngine/JpegeEngine(Device) > DstEngine(Host)

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

版本查询方法，在Atlas产品环境下，运行一下命令：
```bash
npu-smi info
```

## 编译

编译Atlas300 (Model 3000) 或 Atlas300 (Model 3010)程序:
```bash
./build.sh A300
```

编译Atlas 500 (Model 3010)程序:
```bash
./build.sh A500
```

注意: 编译脚本build.sh的默认参数为"A300". 因此，以下的脚本命令默认编译Atlas300程序:
```bash
./build.sh 
```

## 运行

查看帮助信息:
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
