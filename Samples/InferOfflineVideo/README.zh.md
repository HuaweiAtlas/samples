中文|[英文](README.md)
# InferOfflineVideo

## 介绍

本开发样例演示 `InferOfflineVideo` 程序，使用芯片进行 `SSD` 检测和 `resnet50` 分类。

```
StreamPuller(Host) > VDecEngine(Device) > SSDDetection(Device) > ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)
```

## 支持的产品

Atlas 200 (Model 3010), Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

版本号查询方法，在Atlas产品环境下，运行以下命令：
```bash
npu-smi info
```

## 依赖

- ffmpeg 4.1

源码下载地址：https://github.com/FFmpeg/FFmpeg/releases

ffmpeg源码编译和安装配置的方法， 可以参考Ascend开发者论坛：https://bbs.huaweicloud.com/forum/thread-25834-1-1.html，
更多Atlas产品交叉编译的方法请参考产品开发指南，或者自行搜索网上资源。

完成安装配置后，需要导出 `ffmpeg` 的路径到环境变量 `FFMPEG_PATH`。路径下应该包含 `include` 和 `lib` 文件夹。

```bash
export FFMPEG_PATH=/path/to/ffmpeg
```

- 模型转换

参考data\models\README.md下载并转换相关模型。


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

编译 Atlas 200 (Model 3010) 程序
```bash
# 如果未安装 A200 的交叉编译器，请先安装编译器
# apt-get install g++-aarch64-linux-gnu gcc-aarch64-linux-gnu
# 另外也需要配套编译的 ffmpeg，并指定路径
# export FFMPEG_PATH=/path/to/ffmpeg/of/A200
# DDK 指定
# export DDK_HOME=/path/to/ddk/of/A200
./build.sh A200
```

## 配置

需要在 `graph.config` 配置路径信息

修改视频流地址

```bash
 engines {
    id: 101
    engine_name: "StreamPuller"
    ...
      items {
        name: "stream_name"
        value: "rtsp://xx.xx.xx.xx/AmercaTest.264" # 改为实际的拉流地址
      }
    ...
  }
```

修改模型的位置
```bash
  engines {
    id: 103
    engine_name: "SSDDetection"
    ...
    ai_config {
      items {
        name: "model"
        value: "../data/models/vgg_ssd_300x300.om" # 改为实际的模型路径
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
          value: "../data/models/resnet50.om"  # 改为实际的模型路径
        }
      ...
    }
```

修改识别标签文件位置
```bash
  engines {
    id: 106
    engine_name: "DstEngine"
    ...
          ai_config {
                items {
                    name: "labelPath"
                    value: "./imagenet1000_clsidx_to_labels.txt" # 改为实际的标签文件路径
                }
    }
    ...
  }
```

## 运行

```bash
cd out
./main 
```
