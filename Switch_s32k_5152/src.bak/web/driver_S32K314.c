/*
 * driver_S32K314.c
 *
 *  Created on: 2024��10��9��
 *      Author: Wang Xiadong
 */

#include "driver_S32K314.h"
#include "Gmac_Ip.h"
#include "Swt_Ip.h"

/* SWT instance used - 0 */
#define SWT_INST                     0U

bool driver_init_S32K314(struct mg_tcpip_if *ifp){

	return 1;
}

size_t driver_tx_S32K314(const void *buf, size_t len, struct mg_tcpip_if *ifp){
	Gmac_Ip_TxOptionsType TxOptions = {TRUE, GMAC_CRC_AND_PAD_INSERTION, GMAC_CHECKSUM_INSERTION_DISABLE};
	Gmac_Ip_BufferType TxBuffer = {0};
	Gmac_Ip_TxInfoType TxInfo  = {0};
	Gmac_Ip_StatusType gmac_status;
	/* Request a buffer of at least 64 bytes */
	if ((GMAC_STATUS_SUCCESS != Gmac_Ip_GetTxBuff(INST_GMAC_0, 0U, &TxBuffer, NULL_PTR)) || (TxBuffer.Length < len))
		return 0;
	memcpy(TxBuffer.Data,buf,len);
	TxBuffer.Length=len;
	gmac_status = Gmac_Ip_SendFrame(INST_GMAC_0, 0U, &TxBuffer, &TxOptions);
	if(gmac_status != GMAC_STATUS_SUCCESS)
		return 0;
	/* Wait for the frame to be transmitted */
	do {
		gmac_status = Gmac_Ip_GetTransmitStatus(INST_GMAC_0, 0U, &TxBuffer, &TxInfo);
	} while (gmac_status == GMAC_STATUS_BUSY);
	/* Check the frame status */
	if ((GMAC_STATUS_SUCCESS != gmac_status) || (0U != TxInfo.ErrMask))
		return 0;
	return len;
}

size_t driver_rx_S32K314(void *buf, size_t len, struct mg_tcpip_if *ifp){
	Gmac_Ip_BufferType RxBuffer = {0};
	Gmac_Ip_RxInfoType RxInfo  = {0};
	Gmac_Ip_StatusType gmac_status = GMAC_STATUS_RX_QUEUE_EMPTY;
	do {
		gmac_status = Gmac_Ip_ReadFrame(INST_GMAC_0, 0U, &RxBuffer, &RxInfo);
		//
		Swt_Ip_Service(SWT_INST);
	} while (gmac_status == GMAC_STATUS_RX_QUEUE_EMPTY);
	/* Check the frame status */
	if ((GMAC_STATUS_SUCCESS != gmac_status) || (0U != RxInfo.ErrMask))
		return 0;
	memcpy(buf,RxBuffer.Data,RxInfo.PktLen);

	Gmac_Ip_ProvideRxBuff(INST_GMAC_0, 0U, &RxBuffer);

	return RxInfo.PktLen;
}

bool driver_up_S32K314(struct mg_tcpip_if *ifp){
	return 1;
}
