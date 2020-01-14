EN|[CN](README.zh.md)
# DynamicGraph

## Introduction

This sample demonstrates how to use the DynamicGraph program to dynamically create a graph.

```
GreetEngine(Host) > GreetEngine(Device) > GreetEngine(Host)
```

## Supported Products

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3000)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Compilation

Compile the Atlas 300 (Model 3000) or Atlas 300 (Model 3010) program:
```bash
./build.sh A300
```

Compile the Atlas 500 (Model 3000) program:
```bash
./build.sh A500
```

Note: the default run parameter of the compile script build.sh is "A300". It will compile Atlas300 program when running following command:
```bash
./build.sh 
```

## Execution

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

## Note

- `-g` parameter only accept integer between from 1 to 16.
