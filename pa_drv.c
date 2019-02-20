#include <stdint.h>
#include <stdio.h>      /*标准输入输出定义*/  
#include <stdlib.h>     /*标准函数库定义*/  
#include <string.h>
#include <errno.h>      /*错误号定义*/  
#include "pa_drv.h"

#define FALSE 1
#define TRUE 0

uint8_t frame_dat[512];


uint8_t get_sta_send_buff[] = {0x4E,0x01,0xFF,0xFF,0x00,0x00,0x00,0xFF,0x07,0x10,0xD1,0xFF,0x02,0x09,0x02,0x00,0x00,0x03,0x09,0x02,0x00,0x00,0x04,0x09,0x02,0x00,0x00,0x05,0x09,0x02,0x00,0x00,0x06,0x09,0x02,0x00,0x00,0x07,0x09,0x02,0x00,0x00,0x01,0x09,0x01,0x00,0x08,0x09,0x01,0x00,0x00,0x09,0x04,0x00,0x00,0x00,0x00,0x8D,0x2A,0x4E}; /* Default send data */ 
uint8_t get_info_send_buff[] = {0X4E,0x01,0XFF,0XFF,0X00,0X00,0X00,0XFF,
	                            0X02,0X10,0XD1,0XFF,0X02,0X0A,0X20,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
							    0X0A,0X20,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X0A,0X01,0X00,0X03,0X0A,
							    0X20,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
							    0X00,0X04,0X0A,0X01,0X00,0xa6,0x0d,0X4E}; /* Default send data */ 


uint8_t rx_buff[512];

/*status compute data*/
int get_uart_frame(char * device,uint8_t * buff,pPA_STATUS_T st)
{
	st->dc_voltage=(buff[16]<<8)|buff[15];
	st->curr=(buff[21]<<8)|buff[20];
	st->Vgs1_P=(buff[26]<<8)|buff[25];
	st->Vgs1_M=(buff[31]<<8)|buff[30];
	st->Vgs2_P=(buff[36]<<8)|buff[35];
	st->Vgs2_M=(buff[41]<<8)|buff[40];
	st->Temperature=buff[45];
	st->over_temp=buff[49];
	printf("Temperature: %d\n",st->Temperature );
	if(st->over_temp){
		printf("Temperature warning!!!\n");
	}else printf("Temperature normal\n");
}

/*basic information recerve*/
int get_info_frame(char * device,uint8_t * buff,pPA_INFO_T info)
{
	int loop=15;
	int len=0;
	int sum=0;
//	int sw_ver_h,sw_ver_l;
//	int markprint_h,markprint_l;

	for(;loop<128;)
	{
		len=buff[loop-1]+1;
		memcpy(frame_dat+sum, buff+loop,len);
		sum+=len;
		loop+=len+2;
	}
	info->name=frame_dat;

	info->sn=frame_dat+33;

//	sw_ver_h=frame_dat[66]>>4;
//	sw_ver_l=frame_dat[66]&0x0f;
	info->sw_ver=frame_dat[66];

	info->hw_ver=frame_dat+68;

	info->markprint=frame_dat[101];
}


int get_pa_status(char * device , pPA_STATUS_T st)
{
	int fd;
	int nread;
	int len;

	fd=Open_info(device);
	
    len = write(fd, get_sta_send_buff, sizeof(get_sta_send_buff));
    printf("send %d\n",len);
    usleep(13600);

	nread = read(fd, rx_buff, 60);
	printf("receive %d\n",nread );

	if(nread==60&&rx_buff[0]=='N'&&rx_buff[11]==0){
		get_uart_frame(device,rx_buff,st);
		close(fd);
		return 0;
	}else{ 
		close(fd);
		return 1;
		}

}

int set_pa_status(char * device , pPA_STATUS_T st)
{
	int fd;
	int round;

	uint16_t set_data[10];
	scanf("%hd",set_data);

//	for(round=0;round<=10;round++){
	    if(set_data[0]<=5000)
	    {
	    	set_data[1]=((set_data[0]&0xff00)>>8)|((set_data[0]&0x00ff)<<8);
	    }
	    printf("%X\n",set_data[1] );
//	}


}


int get_pa_info(char * device , pPA_INFO_T pinfo)
{
	int fd;
	int nread;
	int len;

	fd=Open_info(device);

	len = write(fd,get_info_send_buff,sizeof(get_info_send_buff));
	printf("send %d\n",len );
	usleep(25530);

	nread = read(fd, rx_buff,128);
	printf("receive %d\n",nread );

	if(nread==128&&rx_buff[0]=='N'&&rx_buff[11]==0){
		get_info_frame(device,rx_buff,pinfo);
		close(fd);
		return 0;
	}else{ 
		close(fd);
		return 1;
		}
}
