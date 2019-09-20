中文|[英文](README.md)
# DecodeVideo

## 介绍

本开发样例演示 `DecodeVideo` 程序，使用芯片自带硬解功能解码 `h264`，`h265` 视频。

```
ScrEngine(Host) > VDecEngine(Device) > DstEngine(Host)
```

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890

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

## 运行

```bash
cd out
./main -h

Usage: main [Options...]

Options:
    -h                             show usage message.
    -i                             input file path
    -d                             dvpp type:
                                       0 - decode h264
                                       1 - decode h265
    -g                             graph count per chip, default is 1
Eg:
    ./main -i /path/to/video/file.264

./main -i /path/to/video/file.264
```

## 注意事项

需用户自行转换视频文件成 `h264` 或 `h265` 文件