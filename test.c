#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
//#include "test.h"


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

struct rfid_format rfid = {
    .card_key0Abuf = {0xff,0xff,0xff,0xff,0xff,0xff},
    .card_writebuf = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
    .BlockAddr = 4,
    };

int main(int argc,char **argv)
{
    int fd_rfid;
    int i;

    fd_rfid=open("/dev/myrfid",O_RDWR);

    if(fd_rfid < 0)
    {
        perror("open /dev/myrfid");
        return -1;
    }


    while(1)
    {	
        ioctl(fd_rfid, CMD_RFID_INIT);   //初始化led

        rfid.status = 0;

        ioctl(fd_rfid, CMD_RFID_Request, &rfid);   //寻卡
        if(rfid.status == 0)    //如果读到卡
        {
            printf("find card!\n");
            ioctl(fd_rfid, CMD_RFID_Anticoll, &rfid);	//防撞处理
            ioctl(fd_rfid, CMD_RFID_SelectTag, &rfid);	//选卡
            ioctl(fd_rfid, CMD_RFID_Auth, &rfid);	//验卡
            ioctl(fd_rfid, CMD_RFID_Write, &rfid);	//写卡（写卡要小心，特别是各区的块3）
            ioctl(fd_rfid, CMD_RFID_Read, &rfid);	//读卡

            //卡类型显示
		    printf("card_pydebuf:%02x %02X\r\n",rfid.card_pydebuf[0],rfid.card_pydebuf[1]);

            //卡序列号显，最后一字节为卡的校验码
            printf("card_numberbuf:");
            for(i=0;i<5;i++)
            {
                printf("%02X ",rfid.card_numberbuf[i]);
            }
            printf("\r\n");
		
            //卡容量显示，单位为Kbits
            
            printf("card_size %dKbits\r\n",rfid.card_size);
            
            //读卡状态显示，正常为0

            printf("status = %d\r\n",rfid.status);
            
            //读一个块的数据显示
            printf("card read:");
            for(i=0;i<18;i++)		//分两行显示
            {
                printf("%02X ",rfid.card_readbuf[i]);
            }   
            printf("\r\n");
            
        }
        sleep(1);
    }

    close(fd_rfid);
    return 0;
}