#ifndef PA_DRV_H
#define PA_DRV_H

#include <stdint.h>

//PA状态结构体
typedef struct PA_status
{
	uint16_t dc_voltage; //28V电源电压
	uint16_t curr;		 //28V电源电流
	uint16_t Vgs1_P;	 //VGS1_P采样电压
	uint16_t Vgs1_M;	 //VGS2_M采样电压
	uint16_t Vgs2_P;	 //VGS1_P采样电压
	uint16_t Vgs2_M;	 //VGS2_M采样电压
	int Temperature;	 //MCU温度
	_Bool over_temp;	 //过温警告

} PA_STATUS_T;

typedef PA_STATUS_T *pPA_STATUS_T;

//PA模块信息结构体
typedef struct PA_info
{
	char *name;		 //设备名称
	char *sn;		 //生产序列号
	int sw_ver;		 //软件版本号
	char *markprint; //软件水印信息
	int hw_ver;		 //硬件版本号

} PA_INFO_T;

typedef PA_INFO_T *pPA_INFO_T;

//PA设置参数信息
typedef struct set_PA_status
{
	uint16_t set_Vgs1_P;
	uint16_t set_Vgs1_M;
	uint16_t set_Vgs2_P;
	uint16_t set_Vgs2_M;
	int set_over_temp;
	int set_work_en;
	int tdd_sw;
	int uldl_mode;

} set_PA_status_T;

typedef set_PA_status_T *pset_PA_status_T;

int get_pa_status(char *device, pPA_STATUS_T st);
int set_pa_status(char *device, pset_PA_status_T st, char *train);

int get_pa_info(char *device, pPA_INFO_T info);

#endif