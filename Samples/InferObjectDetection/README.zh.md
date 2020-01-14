中文|[英文](README.md)

# SSD目标检测开发样例（Sample）


## 介绍    

    本样例主要实现目标检测样例，针对输入的H264/JPG数据，实现视频/图片解码、解码后格式转换、目标检测、输出检测结果。主要涉及VDEC、VPC、JPEGE、AIModelManager的功能。

## 支持的产品

Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

版本号查询方法，在Atlas产品环境下，运行以下命令：
```bash
npu-smi info
```

## 依赖条件

    1.支持单输入、输出层为SSDDetection的目标检测模型，如vgg-ssd, resnet-ssd等模型，其他模型需要需改源码，示例模型请参考<Project Root Folder>/data/README.md获取并转换；
    2.支持huffman编码的JPG图片输入，不支持算术编码，不支持渐进编码，不支持jpeg2000格式；
    3.支持264格式的视频输入，用户可通过ffmpeg将其他格式文件转换为h264格式文件，参考命令如下：
```
    //-bf B帧数目控制，-g 关键帧间隔控制，-s 分辨率控制 -an关闭音频， -r 指定帧率
    ffmpeg -i test.mp4 -vcodec h264 -bf 0 -g 25 -r 10 -s 1280*720 -an -f h264 test1.264
```

##  模型转换

参考[模型转换说明](data/models/README.md)下载并转换相关模型。

## 编译

编译Atlas 800 (Model 3000) 或 Atlas 300 (Model 3010)程序
```bash
export DDK_HOME= <Atlas 300 model 300* DDK_HOME>/ddk
cd <Project Root Folder>
bash build.sh A300
```

编译Atlas 500 (Model 3010)程序
```bash
export DDK_HOME= <Atlas 500 DDK_HOME>/ddk
cd <Project Root Folder>
bash build.sh A500
```

备注: 编译脚本build.sh默认运行参数为"A300"，运行以下命令，将默认编译Atlas300程序。
```bash
./build.sh
```

## 运行

```bash
cd out
./ObjectDetection -h

ObjectDetection [OPTION]
Options:

    -h                             Print a usage message.
    -i '<path>'                    Optional. Specify the input file: jpg iamge or H264 Video, default: test.jpeg
    -g '<graph file>'              Optional. Specify the graph config file, default: graph.config
    -m '<model file>'              Optional. Specify the model file, default: load from graph.config
    -c '<chanel number in graph>'  Optional. Specify the number of video channle in one graph, range [1, 4], default: 1
    -s '<device id>'               start chip id, default is 0
    -e '<device id>'               end chip id, default is 0

Eg: ./ObjectDetection -i ./test.jpg -m ./ObjectDetection.om -g ./graph.config -s 0 -e 1

```


 
