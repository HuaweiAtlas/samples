中文|[英文](README.md)
# DynamicGraph

## 介绍

本开发样例演示 `DynamicGraph` 程序，实现动态创建 `graph`。

```
GreetEngine(Host) > GreetEngine(Device) > GreetEngine(Host)
```

## 支持的产品

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

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

备注: 编译脚本build.sh默认运行参数为"A300"，运行以下命令，将默认编译Atlas300程序。
```bash
./build.sh 
```

## 运行

```bash
cd out
./main -h
Usage: main [Options...]
Options:
    -h                             show usage message.
    -g                             graph count per chip, default is 1, should be in [1, 16]
Eg:
    ./main -g 2

./main -g 2
hello world
hello world
```

## 注意事项

- `-g` 参数只接收 1~16 的整数。 
