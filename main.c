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
#include "pa_drv.h"
#include "cJSON.h"
#include <getopt.h>		//参数传递头文件

// 定义串口
#define tty_node "/dev/ttyUSB0"


int main(int argc, char *argv[])
{

	int status_key = 1;
	int info_key = 1;
	int add;
	int next_option;
	char *st_data;
	cJSON *root;

	char *options = "c";

	while (next_option != -1)
	{
		next_option = getopt(argc, argv, options);
		if (next_option == 'c');
		 off_PA();
	}

	pPA_STATUS_T pst;
	pPA_INFO_T pinfo;
	pset_PA_status_T pset;

	pst = malloc(sizeof(PA_STATUS_T));
	pinfo = malloc(sizeof(PA_INFO_T));
	pset = malloc(sizeof(set_PA_status_T));
	
	//read PA basic status
	for(add=0;status_key==1&&add<20;add++){
		status_key=get_pa_status(tty_node,pst);
	}
//	printf("28V voltage:%d mv\n28V Current:%d mv\nVgs1_P Sampling voltage:%d mv\nVgs1_M Sampling voltage:%d mA\nVgs2_P Sampling voltage:%d mv\nVgs2_M Sampling voltage:%d mv\nTemperature:%d°\n",pst->dc_voltage,pst->curr,pst->Vgs1_P,pst->Vgs1_M,pst->Vgs2_P,pst->Vgs2_M,pst->Temperature);


	//read basic information
	for(add=0;info_key==1&&add<20;add++){
		info_key=get_pa_info(tty_node,pinfo);
	}

//	printf("name:%s\nsw_ver:%d\nsn:%s\nhw_ver:%d\nmarkprint:%s\n",pinfo->name,pinfo->sw_ver,pinfo->sn,pinfo->hw_ver,pinfo->markprint);

	//read
	read_cJSON(pst,pinfo);

//	scanf("%s",st_data);
//	printf("%s\n",st_data);
	root = cJSON_Parse(stdin);
	printJson(tty_node,root,pset);

	exit(0);
}

