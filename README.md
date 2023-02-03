# linux_NFC

说明：

本驱动程序移植自STM32程序。

![](https://files.catbox.moe/0s127n.png)

PCD是接近式卡。PICC是接近式耦合设备。

在通信过程中实际上是使用PCD命令控制RC522发出PICC命令与卡进行交互。

RC522工作过程：

![](https://files.catbox.moe/euqcpx.png)



文件说明：

MFRC522.c MFRC522.h NFC功能函数文件

rfid_main NFC主驱动文件，与MFRC522.c编译成rfid_drv.o 

test.c 驱动测试程序

test 编译好的测试应用程序

Makefile 驱动模块与测试应用程序的编译脚本

rfid_drv.ko 编译好的内核驱动模块
