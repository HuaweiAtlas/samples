EN|[CN](README.zh.md)
# InferClassification

## Introduction

This sample demonstrates how the InferClassification program to use the chip to perform the 'resnet50' target classification.

Process Framework

    ```
    MindInputFile(Host) > JpegDecode(Device) >  ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)
    MindInputFile(Host) > PngDecode(Device)  >  ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)
    ```

## Supported Products

Atlas 300 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## Supported Version

1.3.T33.B890 1.3.2.B893 1.31.T12.B120 1.31.T15.B150

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Dependency

  The input model is Resnet50. Please refer to [model transformation instructions](/data/ readme.md) to transform the model.

## Compilation

Compile the Atlas 300 (Model 3000) or Atlas 300 (Model 3010) program:
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

## configuration

configure path information in 'graph.config'

model path config
```bash
  engines {
    id: 103
    engine_name: "SSDDetection"
    ...
    ai_config {
      items {
        name: "model"
        value: "../data/models/vgg_ssd_300x300.om" # change to your model path
      }
    ...
  }
```

```bash
   engines {
      id: 104
      engine_name: "ObjectClassification"
      ...
      ai_config {
        items {
          name: "model"
          value: "../data/models/resnet50.om" # change to your model path
        }
      ...
    }
```

label file path config
```bash
  engines {
    id: 106
    engine_name: "DstEngine"
    ...
          ai_config {
                items {
                    name: "labelPath"
                    value: "./imagenet1000_clsidx_to_labels.txt" # change to your label file path
                }
    }
    ...
  }
```

## Execution

```bash
cd out
./main -h
```
    -h                         Print a usage message.
    -i                         Optional. Specify the image, default: ../data/test.jpeg.
    -r                         Optional. Input path type, default: 0
                                   0 - Input path is a file.
                                   1 - Input path is a directory.
    -d                         Optional. Specify the dvpp type. default: -1.
                                   0 - Process jpeg
                                   1 - Process png

Classify all Jpeg images in the folder ../data
```bash
cd out
./main -i ../data/ -r 1 -d 0
```

## Constraint
```
Jpeg resolutions: maximum 4096 x 4096, minimum 32 x 32.
Png  resolutions: maximum 4096 x 4096, minimum 32 x 32.
```

## Result
```
The yuv file is generated in the out folder out/result_files.

Print the classification results. The corresponding encoded images and the text files are  generated in the out folder out/result_files.
```
