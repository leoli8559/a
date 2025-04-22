/*
 * server.c
 *
 *  Created on: 2025年1月7日
 *      Author: Wang Xiadong
 */

#include "server.h"
#include "mongoose.h"
#include <driver_S32K314.h>

#define URL 		"http://10.104.3.77:80"
#define MAC_ADDR 	{0x11,0x22,0x33,0x44,0x55,0x66}
#define IPv4_ADDR  	MG_U32(10,104,3,77)
#define MASK 		MG_U32(255,255,255,0)
#define GATEWAY 	MG_U32(10,104,3,255)

extern void http_ev_handler(struct mg_connection *c, int ev, void *ev_data);

//驱动结构体
struct mg_tcpip_driver mg_tcpip_driver_S32K314={
		.init=driver_init_S32K314,
		.tx=driver_tx_S32K314,
		.rx=driver_rx_S32K314,
		.up=driver_up_S32K314 };

void server_task(void *args) {
	(void) args;
	struct mg_mgr mgr;        // Initialize Mongoose event manager
	mg_mgr_init(&mgr);        // and attach it to the interface

	// TCP/IP interface
	struct mg_tcpip_if mif = {
			.mac = MAC_ADDR,
			.ip = mg_htonl(IPv4_ADDR),
			.mask = mg_htonl(MASK),
			.gw = mg_htonl(GATEWAY),
			.driver = &mg_tcpip_driver_S32K314,
			.driver_data = NULL};

	mg_tcpip_init(&mgr, &mif);

	mg_http_listen(&mgr, URL, http_ev_handler, NULL);  // Setup listener

	for (;;) {
		mg_mgr_poll(&mgr, 0);
	}

	vTaskDelete(NULL);
}
