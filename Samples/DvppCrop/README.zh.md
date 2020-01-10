中文|[英文](README.md)
## 介绍

本开发样例演示DvppCrop程序，主要是演示Jpeg图片解码为Yuv格式后，进行抠图并且保存在本地的流程。
流程框架

    DvppCrop: main(Host) > DvppCrop(Device) > SaveFile(Host)

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

版本号查询方法，在Atlas 300产品环境下，运行以下命令：
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
    -h                         Print a usage message.
    -i  '<path>'               Optional. Specify the image, this sample only support jpeg format.
                               Default: ../data/test.jpeg. 
                               Resolutions constraint: maximum 4096 x 4096, minimum 32 x 32.
对图片../data/test.jpeg进行解码和抠图
```bash
cd out
./main -i ../data/test.jpeg
```

## 约束

支持的分辨率：最大分辨率4096 x 4096，最小分辨率 32 x 32。

## 结果

在out文件夹下输出抠图结果res.yuv文件。

