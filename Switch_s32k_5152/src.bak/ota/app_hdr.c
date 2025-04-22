/*
 * app_hdr.c
 *
 *  Created on: 2024��11��20��
 *      Author: Wang Xiadong
 */
#include "app_hdr.h"
#include "CRC_hal.h"

/*Gets the start address of the Flash section of the application*/
extern uint32_t __text_start;

const hseAppHeader_t __attribute__((section("._app_header"))) app_header =
{
        .hdrInfo.hdrTag = APP_HDR_TAG ,
        .hdrInfo.version = APP_VERSION,
        .hdrInfo.subVersion = APP_SUB_VERSION ,
		.hdrInfo.reserved = {0xFF,0XFF} ,
        .hdrInfo.pAppDestAddres = APP_B_START ,
        .hdrInfo.pAppStartEntry = (uint32_t)( &__text_start ) , /*Start Address of the Application*/

		/* ���������ֶ���Ҫ�ֶ���д */
        .hdrInfo.codeLength = 0x497E8, 				/* Application Length */
		.hdrInfo.codeCRC = 0xBFC9,					/* app code crc */
        .hdrCRC = 0x8B76 , 							/* ������������� */

        .reserved2 = {0},
};

bool app_check_hdr(hseAppHeader_t *hdr,uint32_t ricvLen){
	if(hdr->hdrInfo.hdrTag!=APP_HDR_TAG)
		return false;
	if(hdr->hdrInfo.pAppDestAddres!=APP_B_START)
		return false;
	if(hdr->hdrInfo.codeLength!=ricvLen)
		return false;
	uint32_t crc=0;
	CRC_HAL_CalculateCRCOnce((uint8_t*)&hdr->hdrInfo,sizeof(hdr->hdrInfo),&crc);
	if(crc!=hdr->hdrCRC)
		return false;
	return true;
}

