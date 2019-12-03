中文|[英文](README.md)
# HA

## 简介

HA Sample

* 进入src目录，执行makefile进行编译。编译成功后，会在src下面生成明为"mbox-sample"的可执行文件

* 将mbox-sample放入通过FD部署的业务容器中，进入容器并运行程序，输入如下结果：

./mbox-sample
===>>>mbox client start!
1-init mbox
2-start receive message
3-stop receive
4-query ha status
5-force swap
6-send message

启动容器后，输入1回车进行初始化，输入2回车启动接收线程。返回结果如下所示：

please input operation
1
init mbox success!
1-init mbox
2-start receive message
3-stop receive
4-query ha status
5-force swap
6-send message
please input operation
2
==========start recv succeed=============
1-init mbox
2-start receive message
3-stop receive
4-query ha status
5-force swap
6-send message
please input operation
==========begin receive msg=============

此时已经开始接收主备倒换消息，当有倒换消息产生式，会有如下返回：

HA升主：
Recv message===>>>[active notify] 

HA降备：
Recv message===>>>[standby notify]



