/*
 * OTA.c
 *
 *  Created on: 2024年11月15日
 *      Author: Wang Xiadong
 */

#include "OTA.h"
#include "flash.h"
#include "CRC_hal.h"
#include "app_hdr.h"

/* lg=local global var */
static size_t lg_size;	// Firmware size to flash. In-progress indicator
static uint32 lg_crc;	// Firmware checksum
static uint8  lg_check_hdr;	//是否校验app hdr
static uint32 lg_code_crc;	//头部信息中，code_crc字段

bool ota_begin_firmware_update(char *file_name, size_t total_size){
	bool ok = false;
	if (lg_size) {
		//已经begin了
		ok = false;
	} else {
		size_t max = flash_size();
		lg_crc = 0u;
		lg_check_hdr = false;
		if (total_size < max) {
			//有足够空间，可以写入
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
		//验证实际写入FLASH中的与传输的数据是否一致，验证发布的固件有没有被中途篡改
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
		//校验头,注意，S32K3里面的头部信息是包含在APP中的，所以长度也包括了头部信息，不需要校验头部后删去头部长度
		if(!lg_check_hdr){
			if(len>sizeof(hseAppHeader_t)){
				hseAppHeader_t *hdr=buf;
				ok = app_check_hdr(buf,lg_size);
				if(!ok)
					return ok;
				lg_check_hdr=true;
				lg_code_crc=hdr->hdrInfo.codeCRC;
			}else{
				//既没校验头，长度又不够，说明是错误固件
				return ok;
			}
		}
		ok = flash_write(buf,len);
		if(ok)
			CRC_HAL_CreatSoftwareCrc((const uint8_t*)buf, len, &lg_crc);  // Update CRC
	}
	return ok;
}
