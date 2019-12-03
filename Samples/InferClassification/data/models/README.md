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