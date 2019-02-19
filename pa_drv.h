#ifndef PA_DRV_H
#define PA_DRV_H 

#include <stdint.h>

typedef struct PA_status
{
	uint16_t dc_voltage;
	uint16_t curr;
	uint16_t Vgs1_P;
	uint16_t Vgs1_M;
	uint16_t Vgs2_P;
	uint16_t Vgs2_M;
	int Temperature;
	_Bool over_temp;

}PA_STATUS_T;

typedef  PA_STATUS_T * pPA_STATUS_T;


typedef struct PA_info
{
	char * name;
	char * sw_ver;
	char * hw_ver;
	char * markprint;

}PA_INFO_T;

typedef  PA_INFO_T * pPA_INFO_T;



int get_pa_status(char * device , pPA_STATUS_T st);
int set_pa_status(char * device , pPA_STATUS_T st);

int get_pa_info(char * device , pPA_INFO_T info);






#endif