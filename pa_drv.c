#include <stdint.h>
#include <stdio.h>      /*标准输入输出定义*/  
#include <stdlib.h>     /*标准函数库定义*/  
#include <string.h>
#include <errno.h>      /*错误号定义*/  
#include "pa_drv.h"

#define FALSE 1
#define TRUE 0


uint8_t frame_dat[512];
int frame_len;


uint8_t get_sta_send_buff[] = {0x4E,0x01,0xFF,0xFF,0x00,0x00,0x00,0xFF,0x07,0x10,0xD1,0xFF,0x02,0x09,0x02,0x00,0x00,0x03,0x09,0x02,0x00,0x00,0x04,0x09,0x02,0x00,0x00,0x05,0x09,0x02,0x00,0x00,0x06,0x09,0x02,0x00,0x00,0x07,0x09,0x02,0x00,0x00,0x01,0x09,0x01,0x00,0x08,0x09,0x01,0x00,0x00,0x09,0x04,0x00,0x00,0x00,0x00,0x8D,0x2A,0xE4,0x41,0x4E}; /* Default send data */ 
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


int get_uart_frame(char * device,char * buff,char * data)
{
	int len;
	if(buff[0]=='N'&&buff[11]==0){
			int loop=15,max=0,sum=0;
			while(sum!=18){
			    len=buff[loop-1];
			    memcpy(data+sum,buff+loop,len);
			    sum+=len;
			    loop+=3+len;
			    }
		}
		printf("%d,%X,%X,%X,%X,%X\n",data[0] ,data[1] ,buff[15] ,buff[16] ,data[4] ,data[5]);
}


int compute_frame(char * data_status,pPA_STATUS_T st)
{
	st->dc_voltage=data_status[1]*16*16+data_status[0];
	st->curr=data_status[3]*16*16+data_status[2];
	st->Vgs1_P=data_status[5]*16*16+data_status[4];
	st->Vgs1_M=data_status[7]*16*16+data_status[6];
	st->Vgs2_P=data_status[9]*16*16+data_status[8];
	st->Vgs2_M=data_status[11]*16*16+data_status[10];
	st->Temperature=data_status[12];
	st->over_temp=data_status[13];
	printf("\n V=%d mv A=%d mA\n",st->dc_voltage,st->curr);
	printf("Temperature: %d\n",st->Temperature );
	if(st->over_temp){
		printf("Temperature warning!!!\n");
	}else printf("Temperature normal\n");
}


int get_pa_status(char * device , pPA_STATUS_T st)
{
	int fd;
	int nread;
	int len;

	fd = OpenDev(device);
	if (fd > 0) {
		set_speed(fd);
	} else {
		exit(1);
	}

	if (set_Parity(fd,8,1,'N')== FALSE) {
		close(fd);
		exit(1);
	}
	
    len = write(fd, get_sta_send_buff, sizeof(get_sta_send_buff));
    printf("send %d\n",len);
    sleep(2);

	nread = read(fd, rx_buff, 512);
	if(nread!=-1){
		get_uart_frame(device,rx_buff,frame_dat);
		compute_frame(frame_dat,st);
	}

	close(fd);

}

int set_pa_status(char * device , pPA_STATUS_T st)
{

}


int get_pa_info(char * device , pPA_INFO_T info)
{

}
