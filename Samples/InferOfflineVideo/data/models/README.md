## Object Detection Model

##### Train Model Project:

https://github.com/weiliu89/caffe/tree/ssd

##### Original Network Link and Pre-trained Model Link:

https://drive.google.com/file/d/0BzKzrI_SkD1_NDlVeFJDc2tIU1k/view?usp=drivesdk

##### Dependency

Change the type of layer "detection_out", from "DetectionOutput" to "SSDDetectionOutput", in model file -- deploy.prototxt

##### Convert caffe mode To Ascend om file
```bash
omg --framework 0 --model ./deploy.prototxt --weight ./VGG_coco_SSD_300x300.caffemodel --output vgg_ssd_300x300 --insert_op_conf aipp_vgg.cfg
```
## Classification Model

##### Original Network Link

https://github.com/KaimingHe/deep-residual-networks/blob/master/prototxt/ResNet-50-deploy.prototxt

#### Pre-trained Model Link:

https://onedrive.live.com/?authkey=%21AAFW2-FVoxeVRck&id=4006CBB8476FF777%2117887&cid=4006CBB8476FF777

##### Dependency

##### Convert caffe mode To Ascend om file

```bash
omg --framework 0 --model ./ResNet-50-deploy.prototxt --weight ./ResNet-50-model.caffemodel --output resnet50 --insert_op_conf aipp_resnet50.cfg
```

##### Versions that have been verified: 

- Atlas 300
- Atlas 500