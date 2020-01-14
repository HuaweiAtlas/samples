EN|[CN](README.zh.md)
# InferOfflineVideo

## Introduction

This sample demonstrates how to use the HelloDaVinci program to obtain the string information from the device and save the information as a file on the host.
Process Framework

    StreamPuller(Host) > VDecEngine(Device) > SSDDetection(Device) > ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)

## Supported Products

Atlas 200 (Model 3000), Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Dependency

- ffmpeg 4.1

To compile and install ffmpeg with  source code, you can refer to Ascend developers BBS: https://bbs.huaweicloud.com/forum/thread-25834-1-1.html,

For more information on Atlas cross-compilation, please refer to the product development guide, or search for resources in the internet.


The path of 'ffmpeg' needs to be exported to the environment variable 'FFMPEG_PATH'. The path should contain 'include' and 'lib' folders.

```bash
export FFMPEG_PATH=/path/to/ffmpeg
```

## model convert

Download and convert related models by referring to [model transformation instructions](data/models/README.md).

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

Compile the Atlas 200 (Model 3000) program:
```bash
# 如果未安装 A200 的交叉编译器，请先安装编译器
# if you do not have cross compiler for A200, please install before
# apt-get install g++-aarch64-linux-gnu gcc-aarch64-linux-gnu
# 另外也需要配套编译的 ffmpeg，并指定路径
# ffmpeg for A200 is still needed, and set the environment variable
# export FFMPEG_PATH=/path/to/ffmpeg/of/A200
# set DDK_HOME
# export DDK_HOME=/path/to/ddk/of/A200
./build.sh A200
```

## Configuration

configure path information in 'graph.config'

video stream address config

```bash
 engines {
    id: 101
    engine_name: "StreamPuller"
    ...
      items {
        name: "stream_name"
        value: "rtsp://xx.xx.xx.xx/AmercaTest.264" # change to your address
      }
    ...
  }
```

model path config
```bash
  engines {
    id: 103
    engine_name: "SSDDetection"
    ...
    ai_config {
      items {
        name: "model"
        value: "../data/models/vgg_ssd_300x300.om" # change to your model path
      }
    ...
  }
```

```bash
   engines {
      id: 104
      engine_name: "ObjectClassification"
      ...
      ai_config {
        items {
          name: "model"
          value: "../data/models/resnet50.om" # change to your model path
        }
      ...
    }
```

label file path config
```bash
  engines {
    id: 106
    engine_name: "DstEngine"
    ...
          ai_config {
                items {
                    name: "labelPath"
                    value: "./imagenet1000_clsidx_to_labels.txt" # change to your label file path
                }
    }
    ...
  }
```

## Execution

```bash
cd out
./main
```
