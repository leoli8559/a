/*
 * app.h
 *
 *  Created on: 2024.11.15
 *      Author: Wang Xiadong
 */

#ifndef APP_HDR_H_
#define APP_HDR_H_

#include "global.h"

/* Structure of the Application boot header*/
typedef struct
{
    struct{
        uint32_t                hdrTag;                  /**< @brief App header tag shall be 0xAABBCCDD. */
        uint8_t                 version;            	 /**< @brief 主版本号. */
        uint8_t                 subVersion;              /**< @brief 副版本号. */
        uint8_t					reserved[2];			 /**< @brief Reserved field */
        uint32_t                pAppDestAddres;          /**< @brief The destination address where the application is copied.                                                                 This field must be 0x00600000. */
        uint32_t                pAppStartEntry;          /**< @brief The address of the first instruction to be executed.*/
        uint32_t                codeLength;              /**< @brief Length of application image. */
        uint32_t 				codeCRC;				 /**< @brief crc code of application image. */
    }hdrInfo;
    uint32_t					hdrCRC;					 /**< @brief App hdr CRC. */
    uint8_t                 	reserved2[36];           /**< @brief Reserved field has no impact. Set to all zeroes. */
} hseAppHeader_t;	//64B

bool app_check_hdr(hseAppHeader_t *hdr,uint32_t ricvLen);

#endif /* APP_HDR_H_ */
