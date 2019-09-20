Chinese|[English](README.md)
# HA

## Overview

HA Sample

* Go to the src directory and run the **makefile** command to compile the code. After the compilation is successful, an executable file named "mbox-sample" is generated in src.

* Place the **mbox-sample** file in the service container deployed through the FD. Access the container and run the program. Enter the following information:

./mbox-sample
===>>>mbox client start!
1-init mbox
2-start receive message
3-stop receive
4-query ha status
5-force swap
6-send message

After the container is started, enter **1** and press **Enter** to initialize the container. Enter **2** and press **Enter** to start the receiving thread. The result is as follows:

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

At this time, the active/standby switchover message is received. When the switchover message is generated, the following information is returned:

Upgrade to active:
Recv message===>>>[active notify] 

Downgrade to standby:
Recv message===>>>[standby notify]



