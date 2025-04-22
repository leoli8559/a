/*
 * driver_S32K314.h
 *
 *  Created on: 2024��10��9��
 *      Author: Wang Xiadong
 */

#ifndef DRIVER_S32K314_H_
#define DRIVER_S32K314_H_

#include "mongoose.h"

bool driver_init_S32K314(struct mg_tcpip_if *ifp);

size_t driver_tx_S32K314(const void *buf, size_t len, struct mg_tcpip_if *ifp);   // Transmit frame

size_t driver_rx_S32K314(void *buf, size_t len, struct mg_tcpip_if *ifp);  // Receive frame

bool driver_up_S32K314(struct mg_tcpip_if *ifp);

#endif /* DRIVER_S32K314_H_ */
