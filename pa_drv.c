#include <stdint.h>
#include <stdio.h>  /*标准输入输出定义*/
#include <stdlib.h> /*标准函数库定义*/
#include <string.h>
#include <errno.h> /*错误号定义*/
#include "pa_drv.h"
#include "crc16.h"
#include "cJSON.h" //cJSON

//crc16 list
static const uint16_t crc16tab[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

uint8_t frame_dat[512];

uint8_t set_data[12] = {0x4E, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF,
						0x00, 0x00, 0xD2, 0xFF};
uint8_t up_data[51];

uint8_t Vgs1_P[5] = {0x00, 0x08, 0x02, 0x00, 0x00};
uint8_t Vgs1_M[5] = {0x01, 0x08, 0x02, 0x00, 0x00};
uint8_t Vgs2_P[5] = {0x02, 0x08, 0x02, 0x00, 0x00};
uint8_t Vgs2_M[5] = {0x03, 0x08, 0x02, 0x00, 0x00};
uint8_t temp[4] = {0x07, 0x08, 0x01, 0x00};
uint8_t work[4] = {0x06, 0x08, 0x01, 0x00};

uint8_t get_sta_send_buff[] = {0x4E, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF,
							   0x07, 0x10, 0xD1, 0xFF, 0x02, 0x09, 0x02, 0x00,
							   0x00, 0x03, 0x09, 0x02, 0x00, 0x00, 0x04, 0x09,
							   0x02, 0x00, 0x00, 0x05, 0x09, 0x02, 0x00, 0x00,
							   0x06, 0x09, 0x02, 0x00, 0x00, 0x07, 0x09, 0x02,
							   0x00, 0x00, 0x01, 0x09, 0x01, 0x00, 0x08, 0x09,
							   0x01, 0x00, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
							   0x00, 0x8D, 0x2A, 0x4E}; /* Default send status data */

uint8_t get_info_send_buff[] = {0x4E, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF,
								0x02, 0x10, 0xD1, 0xFF, 0x02, 0x0A, 0x20, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
								0x0A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x0A, 0x01, 0x00, 0x03, 0x0A,
								0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x04, 0x0A, 0x01, 0x00, 0xa6, 0x0d, 0x4E}; /* Default send information data */

uint8_t rx_buff[512];

void hexdump(const void *_data, size_t size)
{
	char lt = 0x2E;

	const uint8_t *data = (const uint8_t *)_data;
	size_t offset = 0;

	//printf("\rbase addr: 0x%08X\n  ", _data);
	while (offset < size)
	{
		//printf("\r0x%04X  ", (uint32_t)offset);
		printf("\r0x%08X  ", (uint32_t)(long)(data + offset));
		size_t n = size - offset;
		if (n > 16)
		{
			n = 16;
		}
		for (size_t i = 0; i < 16; ++i)
		{
			if (i == 8)
			{
				printf(" ");
			}

			if (offset + i < size)
			{
				printf("%02X ", data[offset + i]);
			}
			else
			{
				printf("   ");
			}
		}
		printf(" ");
		for (size_t i = 0; i < n; ++i)
		{
			if ((data[offset + i] >= 32) && (data[offset + i] <= 126))
			{ //ASCII范围
				printf("%c", data[offset + i]);
			}
			else
			{
				printf("%c", lt);
			}
		}
		offset += 16;
		printf("\n");
	}
	printf("\n");
}

/*status compute data*/
int get_uart_frame(char *device, uint8_t *buff, pPA_STATUS_T st)
{
	st->dc_voltage = (buff[16] << 8) | buff[15];
	st->curr = (buff[21] << 8) | buff[20];
	st->Vgs1_P = (buff[26] << 8) | buff[25];
	st->Vgs1_M = (buff[31] << 8) | buff[30];
	st->Vgs2_P = (buff[36] << 8) | buff[35];
	st->Vgs2_M = (buff[41] << 8) | buff[40];
	st->Temperature = buff[45];
	st->over_temp = buff[49];
}

/*basic information recerve*/
int get_info_frame(char *device, uint8_t *buff, pPA_INFO_T info)
{
	int loop = 15;
	int len = 0;
	int sum = 0;
	//	int sw_ver_h,sw_ver_l;
	//	int markprint_h,markprint_l;

	for (; loop < 128;)
	{
		len = buff[loop - 1] + 1;
		memcpy(frame_dat + sum, buff + loop, len);
		sum += len;
		loop += len + 2;
	}
	info->name = frame_dat;

	info->sn = frame_dat + 33;

	//	sw_ver_h=frame_dat[66]>>4;
	//	sw_ver_l=frame_dat[66]&0x0f;
	info->sw_ver = frame_dat[66];

	info->markprint = frame_dat + 68;

	info->hw_ver = frame_dat[101];
}

int get_pa_status(char *device, pPA_STATUS_T st)
{
	int fd;
	int nread;
	int len;

	fd = Open_info(device);

	len = write(fd, get_sta_send_buff, sizeof(get_sta_send_buff));
	//	printf("send %d\n",len);
	usleep(13600);

	nread = read(fd, rx_buff, 60);
	//	printf("receive %d\n",nread );

	if (nread == 60 && rx_buff[0] == 'N' && rx_buff[11] == 0)
	{
		get_uart_frame(device, rx_buff, st);
		close(fd);
		return 0;
	}
	else
	{
		close(fd);
		return 1;
	}
}

uint16_t crc16(const char *buf, int len)
{
	int counter;
	uint16_t crc = 0;
	for (counter = 0; counter < len; counter++)
		crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ *buf++) & 0x00FF];
	return crc;
}

int set_pa_status(char *device, pset_PA_status_T st, char *train)
{
	int fd, i, k;
	int len;
	int nread;

	uint8_t Vgs1_P_l, Vgs1_M_l, Vgs2_P_l, Vgs2_M_l;
	uint8_t Vgs1_P_h, Vgs1_M_h, Vgs2_P_h, Vgs2_M_h;
	uint16_t crc_16;
	uint8_t crc16_l, crc16_h;

	memcpy(up_data, set_data, 12);
	i = 12;

	if (train[0] == 1)
	{
		Vgs1_P_l = st->set_Vgs1_P & 0x00FF;
		Vgs1_P_h = (st->set_Vgs1_P & 0xFF00) >> 8;
		Vgs1_P[3] = Vgs1_P_l;
		Vgs1_P[4] = Vgs1_P_h;
		memcpy(up_data + i, Vgs1_P, 5);
		i += 5;
	}

	if (train[1] == 1)
	{
		Vgs1_M_l = st->set_Vgs1_M & 0x00FF;
		Vgs1_M_h = (st->set_Vgs1_M & 0xFF00) >> 8;
		Vgs1_M[3] = Vgs1_M_l;
		Vgs1_M[4] = Vgs1_M_h;
		memcpy(up_data + i, Vgs1_M, 5);
		i += 5;
	}

	if (train[2] == 1)
	{
		Vgs2_P_l = st->set_Vgs2_P & 0x00FF;
		Vgs2_P_h = (st->set_Vgs2_P & 0xFF00) >> 8;
		Vgs2_P[3] = Vgs2_P_l;
		Vgs2_P[4] = Vgs2_P_h;
		memcpy(up_data + i, Vgs2_P, 5);
		i += 5;
	}

	if (train[3] == 1)
	{
		Vgs2_M_l = st->set_Vgs2_M & 0x00FF;
		Vgs2_M_h = (st->set_Vgs2_M & 0xFF00) >> 8;
		Vgs2_M[3] = Vgs2_M_l;
		Vgs2_M[4] = Vgs2_M_h;
		memcpy(up_data + i, Vgs2_M, 5);
		i += 5;
	}

	if (train[4] == 1)
	{
		work[3] = st->set_work_en;
		memcpy(up_data + i, work, 4);
		i += 4;
	}

	if (train[5] == 1)
	{
		temp[3] = st->set_over_temp;
		memcpy(up_data + i, temp, 4);
		i += 4;
	}

	crc_16 = crc16(up_data + 1, i - 1);
	printf("%d\n", i);
	crc16_l = crc_16 & 0x00FF;
	crc16_h = (crc_16 & 0xFF00) >> 8;
	up_data[i] = crc16_l;
	printf("%d\n", i);
	up_data[i + 1] = crc16_h;
	printf("%d\n", i);
	up_data[i + 2] = 0x4E;
	printf("%d\n", i);

	for (k = 0; k < 51; k++)
		printf("%X\t", up_data[k]);
		setbuf(stdin, NULL);

	fd = Open_info(device);

	len = write(fd, up_data, i+3);
	printf("send %d\n", len);
	usleep(12000);

	close(fd);
	return 0;
}

int get_pa_info(char *device, pPA_INFO_T pinfo)
{
	int fd;
	int nread;
	int len;

	fd = Open_info(device);

	len = write(fd, get_info_send_buff, sizeof(get_info_send_buff));
	//	printf("send %d\n",len );
	usleep(25530);

	nread = read(fd, rx_buff, 128);
	//	printf("receive %d\n",nread );

	if (nread == 128 && rx_buff[0] == 'N' && rx_buff[11] == 0)
	{
		get_info_frame(device, rx_buff, pinfo);
		close(fd);
		return 0;
	}
	else
	{
		close(fd);
		return 1;
	}
}

// upload cJSON
int read_cJSON(pPA_STATUS_T st, pPA_INFO_T pst)
{
	cJSON *ret_js = cJSON_CreateObject();
	//	cJSON_AddItemToObject(ret_js, "Production serial", cJSON_CreateString(pst->sn));//未开放

	cJSON_AddItemToObject(ret_js, "Name", cJSON_CreateString(pst->name));				 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Soft_Ver", cJSON_CreateNumber(pst->sw_ver));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Soft_markprint", cJSON_CreateString(pst->markprint)); //软件水印信息
	cJSON_AddItemToObject(ret_js, "Hard_Ver", cJSON_CreateNumber(pst->hw_ver));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "DC_v", cJSON_CreateNumber(st->dc_voltage));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "DC_C", cJSON_CreateNumber(st->curr));				 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Vgs1_P_v", cJSON_CreateNumber(st->Vgs1_P));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Vgs1_M_v", cJSON_CreateNumber(st->Vgs1_M));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Vgs2_P_v", cJSON_CreateNumber(st->Vgs2_P));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Vgs2_M_v", cJSON_CreateNumber(st->Vgs2_M));			 //根节点下添加
	cJSON_AddItemToObject(ret_js, "Temperature", cJSON_CreateNumber(st->Temperature));   //根节点下添加

	printf("%s\n", cJSON_Print(ret_js));

	printf("%s\n", cJSON_PrintUnformatted(ret_js));
	return 0;
}

int off_PA(void)
{
	// set_data[23] = 0x00;
}

//接收cJSON解析数据
int printJson(char *tty_node, cJSON *root, pset_PA_status_T st) //以递归的方式打印json的最内层键值对
{
	uint8_t faker[7];

	if (!root)
	{
		printf("get root faild !\n");
		return -1;
	}

	//Vgs1_P_v解码
	cJSON *Vgs1_P_v = cJSON_GetObjectItem(root, "Vgs1_P_v");
	if (!Vgs1_P_v)
	{
		printf("No Vgs1_P_v !\n");
		faker[0] = 0;
	}
	else
	{
		st->set_Vgs1_P = Vgs1_P_v->valueint;
		faker[0] = 1;
	}
	printf("Vgs1_P_v is %ld\n", st->set_Vgs1_P);

	//Vgs1_M_v解码
	cJSON *Vgs1_M_v = cJSON_GetObjectItem(root, "Vgs1_M_v");
	if (!Vgs1_M_v)
	{
		printf("No Vgs1_M_v !\n");
		faker[1] = 0;
	}
	else
	{
		st->set_Vgs1_M = Vgs1_M_v->valueint;
		faker[1] = 1;
	}
	printf("Vgs1_M_v is %d\n", st->set_Vgs1_M);

	//Vgs2_P_v解码
	cJSON *Vgs2_P_v = cJSON_GetObjectItem(root, "Vgs2_P_v");
	if (!Vgs2_P_v)
	{
		printf("No Vgs2_P_v !\n");
		faker[2] = 0;
	}
	else
	{
		st->set_Vgs2_P = Vgs2_P_v->valueint;
		faker[2] = 1;
	}
	printf("Vgs2_P_v is %d\n", st->set_Vgs2_P);

	//Vgs2_M_v解码
	cJSON *Vgs2_M_v = cJSON_GetObjectItem(root, "Vgs2_M_v");
	if (!Vgs2_M_v)
	{
		printf("No Vgs2_M_v !\n");
		faker[3] = 0;
	}
	else
	{
		st->set_Vgs2_M = Vgs2_M_v->valueint;
		faker[3] = 1;
	}
	printf("Vgs2_M_v is %d\n", st->set_Vgs2_M);

	//PA的开关
	cJSON *work_en = cJSON_GetObjectItem(root, "work_en");
	if (!work_en)
	{
		printf("No work_en !\n");
		faker[4] = 0;
	}
	else
	{
		st->set_work_en = work_en->valueint;
		faker[4] = 1;
	}
	printf("work_en is %ld\n", st->set_work_en);

	//PA的温度报警设置
	cJSON *Temperature = cJSON_GetObjectItem(root, "Temperature");
	if (!Temperature)
	{
		printf("No Temperature !\n");
		faker[5] = 0;
	}
	else
	{
		st->set_over_temp = Temperature->valueint;
		faker[5] = 1;
	}
	printf("Temperature is %ld\n", st->set_over_temp);

	set_pa_status(tty_node, st, faker);

	return 0;
}
