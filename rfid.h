#ifndef __RFID_H
#define __RFID_H			
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>  	 
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <cfg_type.h>
#include <linux/hid.h>   	
						  
//-----------------RFID SPI端口定义----------------  	
//TX1 SCK   PD19
//RX1 CS    PD15
//TX2 MOSI  PD20
//RX2 MISO  PD16
//TX3 RST   PD21
//RX3 IRQ   PD17
     
#define  SCK_GPIO    (PAD_GPIO_D + 19)
#define  CS_GPIO     (PAD_GPIO_D + 15)
#define  MOSI_GPIO   (PAD_GPIO_D + 20)         
#define  MISO_GPIO   (PAD_GPIO_D + 16)  
#define  RST_GPIO    (PAD_GPIO_D + 21)  
#define  IRQ_GPIO    (PAD_GPIO_D + 17)  
	     

//命令
#define CMD_RFID_INIT           _IO('R',0) 
#define CMD_RFID_Request        _IOW('R',1,struct rfid_format) 
#define CMD_RFID_Anticoll       _IOW('R',2,struct rfid_format) 
#define CMD_RFID_SelectTag      _IOW('R',3,struct rfid_format)
#define CMD_RFID_Auth           _IOW('R',4,struct rfid_format) 
#define CMD_RFID_Write          _IOW('R',5,struct rfid_format) 
#define CMD_RFID_Read           _IOR('R',6,struct rfid_format) 


//命令结构体
struct rfid_format 
{ 
    unsigned char card_pydebuf[2]; 
    unsigned char card_numberbuf[5]; 
    unsigned char card_key0Abuf[6];
    unsigned char card_writebuf[16];
    unsigned char card_readbuf[18];
    unsigned char BlockAddr;
    unsigned char card_size;    
    unsigned char status;
};




//驱动的描述信息： #modinfo  *.ko , 驱动的描述信息并不是必需的。
MODULE_AUTHOR("BH-0@qq.com");                   //驱动的作者
MODULE_DESCRIPTION("rfid of driver");       //驱动的描述
MODULE_LICENSE("GPL");                        //遵循的协议
#endif  


