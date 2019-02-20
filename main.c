//串口相关的头文件  
#include <stdio.h>      /*标准输入输出定义*/  
#include <stdlib.h>     /*标准函数库定义*/  
#include <unistd.h>     /*Unix 标准函数定义*/  
#include <sys/types.h>   
#include <sys/stat.h>     
#include <fcntl.h>      /*文件控制定义*/  
#include <termios.h>    /*PPSIX 终端控制定义*/  
#include <errno.h>      /*错误号定义*/  
#include <string.h> 
#include "crc16.h"
#include "pa_drv.h"


#define FALSE 1
#define TRUE 0
#define MAX 50


#define tty_node "/dev/ttyUSB0"



int main(int argc, char *argv[])
{
	int status_key=1;
	int info_key=1;
	int add;

	pPA_STATUS_T pst;
	pPA_INFO_T pinfo;
	pst = malloc(sizeof(PA_STATUS_T));
	pinfo = malloc(sizeof(PA_INFO_T));
	for(add=0;status_key==1&&add<20;add++){
		status_key=get_pa_status(tty_node,pst);
	}
	printf("%d mv,%d mv,%d mv,%d mA,%d mv\n",pst->Vgs2_M,pst->Vgs1_M,pst->Vgs1_P,pst->curr,pst->dc_voltage);
	for(add=0;info_key==1&&add<20;add++){
		info_key=get_pa_info(tty_node,pinfo);
	}
	printf("name:%s\nsw_ver:%d\nsn:%s\nhw_ver:%s\nmarkprint:%d\n",pinfo->name,pinfo->sw_ver,pinfo->sn,pinfo->hw_ver,pinfo->markprint);

	set_pa_status(tty_node,pinfo);


	exit(0);
}