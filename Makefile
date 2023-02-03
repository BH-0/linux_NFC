obj-m += rfid_drv.o 
rfid_drv-objs := rfid_main.o  MFRC522.o
KERNEL_DIR :=/home/alientek/6818GEC/kernel
CROSS_COMPILE :=/home/alientek/6818GEC/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-
PWD:=$(shell pwd)

default:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_DIR) M=$(PWD) modules
	alg test.c -o test
	rm  *.o *.order .*.cmd  *.mod.c *.symvers .tmp_versions -rf
	@echo "*********************	Bulid complete	************************"
clean:
	rm *.o *.order .*.cmd *.mod.c *.symvers .tmp_versions -rf
