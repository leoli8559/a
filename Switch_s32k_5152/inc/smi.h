/*
 * smi.h
 *
 *  Created on: 2025��1��11��
 *      Author: Dannis Chen
 */

#ifndef SMI_H_
#define SMI_H_



#include <stddef.h>
#include <stdint.h>
#include <PlatformTypes.h>

//#include "t1phy.h"
#include "Siul2_Port_Ip_Cfg.h"
/** Clause 22
 *  ST  OP      PHYADR  REGADR          TA      DATA
 *  ##  ##      #####   #####           ##      #### #### #### ####
 *  01
 *      10| Read
 *      01| Write
 *
 *
 *
 *  Clause 45
 *
 *  ST  OP      PHYADR  DEVTYPE         TA      ADDRESS/DATA
 *  ##  ##      #####   #####           ##      #### #### #### ####
 *  00
 *          AccessType          Device            Access Type | Contents
 *      00| Address      00000| Reserved              Address | Address
 *      01| Write        00001| PMD/PMA               Write   | Write Data
 *      11| Read         00010| WIS                   Read    | Read Data
 *      10| Post Read    00011| PCS                   Read inc| Read Data
 *          inc Addr     00100| PHY XS
 *                       00101| DTE XS
 */


// ��Ӧ��GPIO.
#define MDIO_PIN_                MDIO_CPU_PIN
#define MDIO_PORT_               MDIO_CPU_PORT
#define MDC_PIN_                 MDC_CPU_PIN
#define MDC_PORT_                MDC_CPU_PORT


#define MDIO_DELAY          2600	// us
#define MDIO_READ_DELAY     2600  	// us


/*  Or MII_ADDR_C45 into regnum for read/write on mii_bus to enable the 21 bit
*   IEEE 802.3ae clause 45 addressing mode used by 10GIGE phy chips.
*/



#define MDIO_CL45_ADDR  	0x00
#define MDIO_CL45_WRITE 	0x01
#define MDIO_CL45_READ  	0x11
#define MDIO_CL45_READ_INC 	0x10


#define MDIO_CL22_READ  	0x2
#define MDIO_CL22_WRITE 	0x1


void mdc_port_ini(void);
void mdio_port_write_ini(void);
void mdio_port_read_ini(void);

void mdc_high(void);
void mdc_low(void);

uint8 mdio_get_bit(void);
void mdio_set_bit(uint8 val);

void mdio_write_bit(uint8 val);
void mdio_write_bits_num(unsigned int value ,int nBits);

uint8 mdio_read_bit(void);

int  mdio_read_bits_num( int nBits);


//clause 45
void cl45_mdio_frame_addr(int phyAddr, int device, int regAddr);
void cl45_mdio_frame_write(int phyAddr, int device, int data);
int  cl45_mdio_frame_read(int phyAddr, int device);

void cl45_mdio_write(int phyAddr, int device, int regAddr, int data);
int  cl45_mdio_read(int phyAddr, int device, int regAddr);


// clause 22
void cl22_mdio_write(int phyAddr, int regAddr, int data);
int cl22_mdio_read(int phyAddr, int regAddr);



///**
// * @brief set_mdio_reg get the register and then set one or some bits of it
// * @param t1,
// * @param addr_dev
// * @param reg_num
// * @param val_mask  the bit(s) which will be keep unchange, not to be set, is maked as 1.
// *                  this val_mask will excute AND operation with the original register value, and then OR operation with the value_in.
// *                  for example, if you want to set the 4th and 5th MSB bit, then the other bits should kept unchanged, so the val_mask is 0b1110 0111 1111 1111=0xe7ff.
// *
// * @param val_in    used in or operation , as described in @param val_mask
// * @return          -1 on failure 0 on success
// */
//int set_mdio_reg(struct T1* t1,   unsigned short addr_dev, unsigned short reg_num, unsigned short val_mask, unsigned short val_in);
//
///**
// * @brief set_mdio_reg_whole set the whole register with an unsigned short value.
// * @param t1
// * @param addr_dev
// * @param reg_num
// * @param val_in        the 16-bit value which will be assigned to the register.
// * @return              -1 on failure 0 on success
// */
//int set_mdio_reg_whole(struct T1* t1,   unsigned short addr_dev, unsigned short reg_num, unsigned short val_in);
//
//
///**
// * @brief get_mdio_reg   get the specific bit or bits in the register, the bits are not shifted, still stay where they are.
// * @param t1,
// * @param addr_dev
// * @param reg_num
// * @param val_mask      the bit(s) which is(are) usefull, is set 1.
// *                      for example, if you want to get the 4th MSB bit, the val_mask is 0b0001 0000 0000 0000=0x1000.
// *                      if you want to get the complete value of the register, val_mask should be 0xffff.
// * @return              the 16-bit register value
// */
//unsigned short get_mdio_reg(struct T1* t1,   unsigned short addr_dev, unsigned short reg_num, unsigned short val_mask);




#endif /* SMI_H_ */
