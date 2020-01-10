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

##### Versions that have been verified: 

- Atlas 800 (Model 3000)
- Atlas 300 (Model 3010)
- Atlas 500 (Model 3010)