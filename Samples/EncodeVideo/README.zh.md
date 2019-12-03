中文|[英文](README.md)
## 介绍

本开发样例演示EncodeVideo程序，主要演示yuv文件编码生成.bin文件的流程。
流程框架

    EncodeVideo: main(Host) > VideoIn(Host) > VideoEncode(Device) > VideoOut(Host)

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
    -i '<path>'                    Requested. Specify the input image, eg, /home/data/test.yuv
    -height                        Requested. Specify the height of input image range = [128, 1920]
    -width                         Requested. Specify the width of input image  range = [128, 1920]

```bash
cd out
./main -i=/home/test.yuv -height 960 -width 1280
```
