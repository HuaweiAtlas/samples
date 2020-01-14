EN|[CN](README.zh.md)
# LogSample

## Introduction

This Sample demonstrate how to define and use log in our project based on the HelloDavinci Sample. Data flows as followed:


    ScrEngine(Host) > DeviceEngine(Device) > DstEngine(Host)

## Supported Products

Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

You may run the following command to get the currently installed version of Atlas product in the environment:
```bash
npu-smi info
```

## Compilation

Compile this sample as Atlas 800 (Model 3000) or Atlas 300 (Model 3010) program:
```bash
./build.sh A300
```

Compile this sample as Atlas 500 (Model 3010) program:
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
./main
```

## Result

If everything goes properly, you will see the string "Log Sample finfished! Details please refer to /var/dlog" on terminal, and a file named 'davinci_log_info.txt' will be generated under the out directory in the project, in which writes “This message is from LogSample”. Besides, the defined log can be found in log files under /var/dlog.
