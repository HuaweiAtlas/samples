中文|[英文](README.md)
# HelloDavinci

## 介绍

本开发样例演示HelloDavinci程序，主要是从device侧获取字符串信息，在host保存成文件。
流程框架

    ScrEngine(Host) > HelloWord(Device) > DstEngine(Host)

## 支持的产品

Atlas 200 (Model 3010), Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## 支持的版本

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

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

备注: 脚本默认运行参数为"A300"，运行以下命令，将默认编译Atlas300程序
```bash
./build.sh 
```

编译 Atlas 200 (Model 3010) 程序
```bash
# 如果未安装 A200 的交叉编译器，请先安装编译器
# apt-get install g++-aarch64-linux-gnu gcc-aarch64-linux-gnu
# DDK 指定
# export DDK_HOME=/path/to/ddk/of/A200
./build.sh A200
```

## 运行

```bash
cd out
./main
```

## 结果


在终端中输出“Hello Davinci! ”字样, 在out文件夹下输出davinci_log_info.txt，内容为“This message is from HelloDavinci”.

