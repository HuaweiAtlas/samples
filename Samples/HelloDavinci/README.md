EN|[CN](README.zh.md)
# HelloDavinci

## Introduction

This sample demonstrates how to use the HelloDaVinci program to obtain the string information from the device and save the information as a file on the host.
Process Framework

    ScrEngine(Host) > HelloWord(Device) > DstEngine(Host)

## Supported Products

Atlas 200 (Model 3000), Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Compilation

Compile the Atlas 800 (Model 3000) or Atlas 300 (Model 3010) program:
```bash
./build.sh A300
```

Compile the Atlas 500 (Model 3010) program:
```bash
./build.sh A500
```

Note: the default run parameter of the compile script build.sh is "A300". It will compile Atlas300 program when running following command:
```bash
./build.sh 
```

Compile the Atlas 200 (Model 3000) program:
```bash
# 如果未安装 A200 的交叉编译器，请先安装编译器
# if you do not have cross compiler for A200, please install before
# apt-get install g++-aarch64-linux-gnu gcc-aarch64-linux-gnu
# set DDK_HOME
# export DDK_HOME=/path/to/ddk/of/A200
./build.sh A200

## Execution

```bash
cd out
./main
```

## Result


On the terminal, the message "Hello Davinci!" is displayed. In the out folder, davinci_log_info.txt whose content is "This message is from HelloDavinci" is generated.


