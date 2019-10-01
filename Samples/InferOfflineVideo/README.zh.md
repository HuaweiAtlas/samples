中文|[英文](README.md)
# InferOfflineVideo

## 介绍

本开发样例演示 `InferOfflineVideo` 程序，使用芯片进行 `SSD` 检测和 `resnet50` 分类。

```
StreamPuller(Host) > VDecEngine(Device) > SSDDetection(Device) > ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)
```

## 支持的产品

Atlas 300 (Model 3000)

## 支持的版本

1.3.T33.B890

版本号查询方法，在Atlas产品环境下，运行以下命令：
```bash
npu-smi info
```

## 依赖

- ffmpeg 4.1

需要导出 `ffmpeg` 的路径到环境变量 `FFMPEG_PATH`。路径下应该包含 `include` 和 `lib` 文件夹。

```bash
export FFMPEG_PATH=/path/to/ffmpeg
```

## 编译

编译Atlas300 (Model 3000)程序
```bash
./build.sh A300
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