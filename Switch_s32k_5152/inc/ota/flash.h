/*
 * flash.h
 *
 *  Created on: 2024Äê11ÔÂ15ÈÕ
 *      Author: Wang Xiadong
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "global.h"

bool flash_begin(size_t len);

bool flash_end();

void *flash_APP_A_start(void);

void *flash_APP_B_start(void);

size_t flash_size(void);

size_t flash_sector_size(void);

size_t flash_write_align(void);

int flash_bank(void);

bool flash_erase(size_t len);

bool flash_swap(void);

bool flash_write(const void *buf, size_t len);

//void mg_device_reset(void) {
//}


#endif /* FLASH_H_ */
