中文|[英文](README.md)
# InferClassification

## 介绍

本开发样例演示 `InferClassification` 程序，使用芯片进行 `resnet50` 目标分类。

```
MindInputFile(Host) > JpegDecode(Device) >  ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)
MindInputFile(Host) > PngDecode(Device)  >  ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)
```

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

版本号查询方法，在Atlas产品环境下，运行以下命令：
```bash
npu-smi info
```

## 依赖条件

支持单输入的Resnet50的目标分类模型，示例模型请参考[模型转换说明](/data/README.md)获取并转换


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

## 配置

需要在 `graph.config` 配置路径信息

修改模型的位置
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
查看帮助文档
```bash
cd out
./main -h
```
    -h                         Print a usage message.
    -i                         Optional. Specify the image, default: ../data/test.jpeg.
    -r                         Optional. Input path type, default: 0
                                   0 - Input path is a file.
                                   1 - Input path is a directory.
    -d                         Optional. Specify the dvpp type. default: -1.
                                   0 - Process jpeg
                                   1 - Process png

对文件夹../data下所有的jpeg图片进行推理
```bash
cd out
./main -i ../data/ -r 1 -d 0
```
## 约束
```
Jpeg 分辨率范围: 最大4096 x 4096, 最小32 x 32.
Png  分辨率范围: 最大4096 x 4096, 最小32 x 32.
```

## Result
```
打印识别结果，并在文件夹out/result_files下生成对应编码图片和识别结果的文本文件
```

