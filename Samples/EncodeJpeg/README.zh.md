中文|[英文](README.md)
## 介绍

本开发样例演示EncodeJpeg程序，主要是演示讲yuv文件编码为jpg图片后，保存在本地的流程。
流程框架

    EncodeJpeg: main(Host) > InputFile(Host) > JpegEncode(Device) > SaveFile(Host)

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

版本号查询方法，在Atlas产品环境下，运行以下命令：
```bash
npu-smi info
```

## 编译

编译Atlas300 (Model 3000) 或 Atlas 300 (Model 3010)程序
```bash
./build.sh A300
```

编译Atlas 500 (Model 3010)程序
```bash
./build.sh A500
```

备注: 编译脚本build.sh默认运行参数为"A300"，运行以下命令，将默认编译Atlas300程序。
```bash
./build.sh
```

## 运行
查看帮助文档
```bash
cd out
./main -h
```
    -h                             Print a usage message.
    -height                        Requested. Specify the height of input image
    -width                         Requested. Specify the width of input image
    -format                        Requested. Specify the format of input image, YUV422: {UYVY:0, VYUY:1, YVYU:2, YUYV:3}, YUV420: {NV12:16, NV21:17}
    -i '<path>'                    Optional. Specify the input image, default: ../data/test.yuv
对指定的yuv文件编码为jpg图片并保存为res.jpg
```bash
cd out
./main -i=../data/test.yuv -height 960 -width 1280 -format 16
```
