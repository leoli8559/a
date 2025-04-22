/*
 * OTA.c
 *
 *  Created on: 2024��11��15��
 *      Author: Wang Xiadong
 */

#include "OTA.h"
#include "flash.h"
#include "CRC_hal.h"
#include "app_hdr.h"

/* lg=local global var */
static size_t lg_size;	// Firmware size to flash. In-progress indicator
static uint32 lg_crc;	// Firmware checksum
static uint8  lg_check_hdr;	//�Ƿ�У��app hdr
static uint32 lg_code_crc;	//ͷ����Ϣ�У�code_crc�ֶ�

bool ota_begin_firmware_update(char *file_name, size_t total_size){
	bool ok = false;
	if (lg_size) {
		//�Ѿ�begin��
		ok = false;
	} else {
		size_t max = flash_size();
		lg_crc = 0u;
		lg_check_hdr = false;
		if (total_size < max) {
			//���㹻�ռ䣬����д��
			ok = flash_begin(total_size);
			lg_size = total_size;
		} else {
			ok = false;
		}
	}
	return ok;
}

bool ota_end_firmware_update(void *context){
	uint8 *base = (uint8 *)flash_APP_B_start();
	bool ok = false;
	if (lg_size) {
		uint32 crc=0u;
		CRC_HAL_CalculateCRCOnce(base, lg_size,&crc);
		CRC_HAL_EndSoftwareCrc(&lg_crc);
		//��֤ʵ��д��FLASH�е��봫��������Ƿ�һ�£���֤�����Ĺ̼���û�б���;�۸�
		if (crc == lg_crc && lg_code_crc == crc) {
			ok = flash_end();
		}
		lg_size = 0;
		lg_crc = 0;
		lg_check_hdr = false;
	}
	return ok;
}

bool ota_write_firmware_update(void *context, void *buf, size_t len){
	bool ok = false;
	if (lg_size == 0) {
		return ok;
	} else {
		//У��ͷ,ע�⣬S32K3�����ͷ����Ϣ�ǰ�����APP�еģ����Գ���Ҳ������ͷ����Ϣ������ҪУ��ͷ����ɾȥͷ������
		if(!lg_check_hdr){
			if(len>sizeof(hseAppHeader_t)){
				hseAppHeader_t *hdr=buf;
				ok = app_check_hdr(buf,lg_size);
				if(!ok)
					return ok;
				lg_check_hdr=true;
				lg_code_crc=hdr->hdrInfo.codeCRC;
			}else{
				//��ûУ��ͷ�������ֲ�����˵���Ǵ���̼�
				return ok;
			}
		}
		ok = flash_write(buf,len);
		if(ok)
			CRC_HAL_CreatSoftwareCrc((const uint8_t*)buf, len, &lg_crc);  // Update CRC
	}
	return ok;
}
