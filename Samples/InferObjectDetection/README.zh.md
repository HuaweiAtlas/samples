����|[Ӣ��](README.md)

# SSDĿ���⿪��������Sample��


## ����    

    ��������Ҫʵ��Ŀ������������������H264/JPG���ݣ�ʵ����Ƶ/ͼƬ���롢������ʽת����Ŀ���⡢������������Ҫ�漰VDEC��VPC��JPEGE��AIModelManager�Ĺ��ܡ�

## ֧�ֵĲ�Ʒ

Atlas 800 (Model 3000), Atlas 300 (Model 3010), Atlas 500 (Model 3010)

## ֧�ֵİ汾

1.3.T33.B890 1.3.2.B893 1.3.5.B896 1.31.T12.B120 1.31.T15.B150

�汾�Ų�ѯ��������Atlas��Ʒ�����£������������
```bash
npu-smi info
```

## ��������

    1.֧�ֵ����롢�����ΪSSDDetection��Ŀ����ģ�ͣ���vgg-ssd, resnet-ssd��ģ�ͣ�����ģ����Ҫ���Դ�룬ʾ��ģ����ο�<Project Root Folder>/data/README.md��ȡ��ת����
    2.֧��huffman�����JPGͼƬ���룬��֧���������룬��֧�ֽ������룬��֧��jpeg2000��ʽ��
    3.֧��264��ʽ����Ƶ���룬�û���ͨ��ffmpeg��������ʽ�ļ�ת��Ϊh264��ʽ�ļ����ο��������£�
```
    //-bf B֡��Ŀ���ƣ�-g �ؼ�֡������ƣ�-s �ֱ��ʿ��� -an�ر���Ƶ�� -r ָ��֡��
    ffmpeg -i test.mp4 -vcodec h264 -bf 0 -g 25 -r 10 -s 1280*720 -an -f h264 test1.264
```

##  ģ��ת��

�ο�[ģ��ת��˵��](data/models/README.md)���ز�ת�����ģ�͡�

## ����

����Atlas 800 (Model 3000) �� Atlas 300 (Model 3010)����
```bash
export DDK_HOME= <Atlas 300 model 300* DDK_HOME>/ddk
cd <Project Root Folder>
bash build.sh A300
```

����Atlas 500 (Model 3010)����
```bash
export DDK_HOME= <Atlas 500 DDK_HOME>/ddk
cd <Project Root Folder>
bash build.sh A500
```

��ע: ����ű�build.shĬ�����в���Ϊ"A300"���������������Ĭ�ϱ���Atlas300����
```bash
./build.sh
```

## ����

```bash
cd out
./ObjectDetection -h

ObjectDetection [OPTION]
Options:

    -h                             Print a usage message.
    -i '<path>'                    Optional. Specify the input file: jpg iamge or H264 Video, default: test.jpeg
    -g '<graph file>'              Optional. Specify the graph config file, default: graph.config
    -m '<model file>'              Optional. Specify the model file, default: load from graph.config
    -c '<chanel number in graph>'  Optional. Specify the number of video channle in one graph, range [1, 4], default: 1
    -s '<device id>'               start chip id, default is 0
    -e '<device id>'               end chip id, default is 0

Eg: ./ObjectDetection -i ./test.jpg -m ./ObjectDetection.om -g ./graph.config -s 0 -e 1

```


 
