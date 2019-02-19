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

	pPA_STATUS_T pst;
	pst = malloc(sizeof(PA_STATUS_T));
	get_pa_status(tty_node,pst);
	printf("%d mv,%d mv,%d mv,%d mA,%d mv\n",pst->Vgs2_M,pst->Vgs1_M,pst->Vgs1_P,pst->curr,pst->dc_voltage);

	//printf("dc_voltage%d\n",pst->dc_voltage);
	//printf("curr=    %d\n", pst->curr   );
	//printf("Vgs1_P = %d\n", pst->Vgs1_P );
	//printf("Vgs1_M = %d\n", pst->Vgs1_M );
	//printf("Vgsq_P = %d\n", pst->Vgsq_P );
	//printf("Vgsq_M = %d\n", pst->Vgsq_M );

	//get_uart_frame(tty_node);


	exit(0);
}