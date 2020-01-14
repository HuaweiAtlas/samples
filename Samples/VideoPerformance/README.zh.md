中文|[英文](README.md)
# VideoPerformance

## 介绍

本开发样例演示如何测试Ascend310芯片上的Vdec模块的视频解码性能以及Venc模块的视频编码性能.

流程框架

    SrcEngine(Device) > VDecEngine(Device)/VencEngine(Device) > DstEngine(Host)

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

版本查询方法，在Atlas产品环境下，运行以下命令：
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


## 注意

为了能正确测试性能，测试输入的视频文件包含的图像帧不能少于300帧，除此之外，视频大小不能大于128M。

## 运行

查看帮助信息:
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
运行程序：

```bash
cd out
./main -i ../test.h264 -s 0 -e 3 -t 4 -g 4
```
