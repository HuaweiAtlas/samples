EN|[CN](README.zh.md)
# InferOfflineVideo

## Introduction

This sample demonstrates how to use the HelloDaVinci program to obtain the string information from the device and save the information as a file on the host.
Process Framework

    StreamPuller(Host) > VDecEngine(Device) > SSDDetection(Device) > ObjectClassification(Device) > JpegEncode(Device) > DstEngine(Host)

## Supported Products

Atlas 300 (Model 3000)

## Supported Version

1.3.T33.B890

Run the following command to check the version in the environment where the Atlas product is installed:
```bash
npu-smi info
```

## Dependency

- ffmpeg 4.1

The path of 'ffmpeg' needs to be exported to the environment variable 'FFMPEG_PATH'. The path should contain 'include' and 'lib' folders.

```bash
export FFMPEG_PATH=/path/to/ffmpeg
```

## Compilation

Compile the Atlas 300 (Model 3000) program:
```bash
./build.sh A300
```


## Configuration

configure path information in 'graph.config'

video stream address config

```bash
 engines {
    id: 101
    engine_name: "StreamPuller"
    ...
      items {
        name: "stream_name"
        value: "rtsp://xx.xx.xx.xx/AmercaTest.264" # change to your address
      }
    ...
  }
```

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
./main
```