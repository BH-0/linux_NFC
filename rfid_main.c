#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <cfg_type.h>
#include "rfid.h"
#include "MFRC522.h"

#define DEVICE_NAME     "myrfid"             //设备名字


extern void nxp_soc_gpio_set_io_pull_sel(unsigned int io, int up);	//内部上下拉函数	 

static struct gpio rfid_gpios[] = {
	{ SCK_GPIO,  GPIOF_OUT_INIT_LOW,  "SCK"  }, //由于RC522的SPI工作模式为模式0，因此时钟线在总线空闲的时候为低电平
	{ CS_GPIO,   GPIOF_OUT_INIT_HIGH, "CS"   }, //默认不使能
	{ MOSI_GPIO, GPIOF_OUT_INIT_HIGH, "MOSI" }, 
	{ MISO_GPIO, GPIOF_IN,            "MISO" }, 
	{ RST_GPIO,  GPIOF_OUT_INIT_HIGH, "RST"  }, 
	{ IRQ_GPIO,  GPIOF_IN,            "IRQ"  }, 
};

/*************************************************
*控制函数
*cmd  命令
*args 结构体参数
*************************************************/
static long myrfid_ioctl(struct file *file, unsigned int  cmd, unsigned long args)
{
	struct rfid_format rfid;
	int ret = 0;
	void __user *argp = (void __user *)args;

	if(_IOC_TYPE(cmd) != 'R')
		return -ENOIOCTLCMD;

	if(_IOC_DIR(cmd) == _IOC_READ || _IOC_DIR(cmd) == _IOC_WRITE)
	{
		ret=copy_from_user(&rfid,argp,sizeof(struct rfid_format));
		if(ret)
			return - EFAULT;
	}

	switch(cmd){
		case CMD_RFID_INIT:
			MFRC522_Initializtion();			//初始化MFRC522
			//printk(KERN_INFO"rfid_Init\n");

			break;
		case CMD_RFID_Request:	//寻卡
			rfid.status = MFRC522_Request(0x52, rfid.card_pydebuf);

			break;
		case CMD_RFID_Anticoll:		//防冲撞
			rfid.status = MFRC522_Anticoll(rfid.card_numberbuf);

			break;
		case CMD_RFID_SelectTag:	//选卡
			rfid.card_size = MFRC522_SelectTag(rfid.card_numberbuf);

			break;
		case CMD_RFID_Auth:		//验卡
			rfid.status = MFRC522_Auth(0x60, rfid.BlockAddr, rfid.card_key0Abuf, rfid.card_numberbuf);

			break;
		case CMD_RFID_Write:	//写卡（写卡要小心，特别是各区的块3）
			rfid.status = MFRC522_Write(rfid.BlockAddr, rfid.card_writebuf);

			break;
		case CMD_RFID_Read:		//读卡
			rfid.status=MFRC522_Read(4, rfid.card_readbuf);
			break;

		default:
			printk(KERN_ERR"the cmd is error\n"); 
			return -EINVAL;
			break;
	}

	if(_IOC_DIR(cmd) == _IOC_READ || _IOC_DIR(cmd) == _IOC_WRITE)
	{
		ret=copy_to_user(argp,&rfid,sizeof(struct rfid_format));
		if(ret)
			return - EFAULT;
	}

	return 0;
}

/*************************************************
*打开文件
*************************************************/
int myrfid_open (struct inode *inode, struct file *file)
{
	printk(KERN_INFO"myrfid_open\n");

	return 0;
}

/*************************************************
*关闭文件
*************************************************/
int myrfid_release (struct inode *inode, struct file *file)
{
	printk(KERN_INFO"myrfid_release\n");

	return 0;
}

/*************************************************
*文件操作集
*************************************************/
static const struct file_operations chrdev_fops = {
	.owner	= THIS_MODULE,
	.open =myrfid_open,
	.release = myrfid_release,
	.unlocked_ioctl = myrfid_ioctl,
};

/*************************************************
*杂项设备
*************************************************/
static struct miscdevice rfid_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &chrdev_fops,
};

/********************************************************************
*驱动的初始化函数--->从内核中申请资源（内核、中断、设备号、锁....）
********************************************************************/
static int __init gec6818_rfid_init(void) 
{
	int ret;
	gpio_free_array(rfid_gpios, ARRAY_SIZE(rfid_gpios));	//释放管脚

	ret = gpio_request_array(rfid_gpios, ARRAY_SIZE(rfid_gpios)); //申请一组gpio管脚
	if(ret <0) {
		printk(KERN_ERR"gpio_request_array error\n");
		goto fail_gpio_request;
	}

	nxp_soc_gpio_set_io_pull_sel(CS_GPIO,1);	//设置上拉


	//gpio_direction_output(SDA_GPIO, 1);       //将GPIO设置为输出"1"
	
	ret = misc_register(&rfid_misc);                      //自动生成设备文件
	if(ret <0){
		printk(KERN_ERR"misc_register error\n");
		goto err_misc_register;
	}

	printk(KERN_INFO"myrfid init \r\n");
	return 0;     //驱动注册成功，返回0；驱动注册失败，返回一个负数错误码

//出错处理
err_misc_register:	
	gpio_free_array(rfid_gpios, ARRAY_SIZE(rfid_gpios));
fail_gpio_request:

	return  ret;
}

/*****************************************************************
*驱动退出函数 --->将申请的资源还给内核
*****************************************************************/
static void __exit gec6818_rfid_exit(void)            
{
	gpio_free_array(rfid_gpios, ARRAY_SIZE(rfid_gpios)); //释放引脚资源
	misc_deregister(&rfid_misc);	//注销混杂设备
	printk(KERN_INFO"myrfid exit\n");
}

module_init(gec6818_rfid_init);             //驱动的入口函数会调用一个用户的初始化函数
module_exit(gec6818_rfid_exit);             //驱动的出口函数会调用一个用户的退出函数




