中文|[英文](README.md)
# HelloDavinci

## 介绍

本开发样例演示HelloDavinci程序，主要是从device侧获取字符串信息，在host保存成文件。
流程框架

    ScrEngine(Host) > HelloWord(Device) > DstEngine(Host)

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
./main
```

## 结果


在终端中输出“Hello Davinci! ”字样, 在out文件夹下输出davinci_log_info.txt，内容为“This message is from HelloDavinci”.

