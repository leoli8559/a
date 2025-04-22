/*
 * marvell_88e1512.c
 *
 *  Created on: 2025年1月6日
 *      Author: Wang Xiadong
 */

#include "marvell_88e1512.h"
#include "Gmac_Ip.h"

#include "smi.h"

#define CFG_PHY_CTRL_IDX 0u
#define PHY_ADD 1u
#define PAGE_RAG 22u
#define PHY_ID1 0x0141
#define PHY_ID2 0x0011

/**
 * @Brief 初始化phy 88e1512
 */
int marvell_88e1512_init(){
	Gmac_Ip_StatusType status=0;
	uint16 reg_val;

	mdc_port_ini();

	//test
	for(int i=0;i<320000;i++){
		//选页
		cl22_mdio_write(i,22,0);
		reg_val=cl22_mdio_read(i,22);

//		for(int i=0;i<1600;i++);
//		if(reg_val==10)
//			break;
//		if(reg_val!=0xffff)
//			break;
	}
	//判断PHY id1, marvell OUI is 0x005043, page0_reg2[15:0] store bits 3 to 18, so read uint16[15:0] should be 0x0141
	//判断PHY id2, marvell OUI is 0x005043, page0_reg3[15:10] store bits 19 to 24, so read uint16[15:10] should be b'000011

	//设置速率 100Mbps  page0_reg0  Bit 6,13:01=100Mbps

	//设置全双工 page0_reg0[8] 1=full 0=half

	// MII 模式设置 page18_reg20[2:0] b'000=MII to Copper

	// 读取 查看是否MII模式

	// 复位是设置生效

	return 0;
}
