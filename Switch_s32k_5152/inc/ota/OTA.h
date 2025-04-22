/*
 * OTA.h
 *
 *  Created on: 2024Äê11ÔÂ15ÈÕ
 *      Author: Wang Xiadong
 */

#ifndef OTA_H_
#define OTA_H_

#include "global.h"

bool ota_begin_firmware_update(char *file_name, size_t total_size);

bool ota_end_firmware_update(void *context);

bool ota_write_firmware_update(void *context, void *buf, size_t len);

#endif /* OTA_H_ */
