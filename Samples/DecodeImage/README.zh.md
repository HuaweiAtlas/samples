中文|[英文](README.md)
# DecodeImage

## 介绍

本开发样例演示DecodeImage程序，主要是演示Jpeg或者Png图片解码为Yuv格式后，进行缩放并且保存在本地的流程。
流程框架

    Jpeg Decode Resize: MindInput(Host) > JPEGDResize(Device) > SaveFile(Host)
    Png  Decode Resize: MindInput(Host) > PNGDResize(Device)  > SaveFile(Host)

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
    -h         Print a usage message.
    -i         Optional. Specify the image path, a directory or file. default: ../data/test.jpeg.
    -r         Optional. Input path type. default：0.
                    0 - Input path is a directory.
                    1 - Input path is a file.
    -d         Optional. Specify the dvpp type. default: -1.
                    0 - decode jpeg
                    1 - decode png
               Jpeg resolutions: maximum 8192 x 8192, minimum 32 x 32.
               Png  resolutions: maximum 4096 x 4096, minimum 32 x 32.
               
对文件夹../data下所有的jpeg图片进行解码和缩放
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
在文件夹out/result_files下生成对应yuv文件.
```
