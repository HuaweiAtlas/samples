中文|[英文](README.md)
# LogSample

## 介绍

本开发样例基于HelloDavinci程序演示如何在Host和Device侧定义及打印日志。
流程框架

    ScrEngine(Host) > DeviceEngine(Device) > DstEngine(Host)

## 支持的产品

Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

版本号查询方法，在装有Atlas300的环境下，运行以下命令：
```bash
npu-smi info
```

## 编译

编译Atlas 800 (Model 3000) 或 Atlas 300 (Model 3010)程序
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

```bash
cd out
./main
```

## 结果


在终端中输出“Log Sample finfished! Details please refer to /var/dlog”字样, 在out文件夹下输出davinci_log_info.txt，内容为“This message is from LogSample".日志输出在/var/dlog目录下的日志中。

