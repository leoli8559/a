## TI Confidential - NDA Restrictions
## 
## Copyright 2021 Texas Instruments Incorporated. All rights reserved.
## 
## IMPORTANT: Your use of this Software is limited to those specific rights
## granted under the terms of a software license agreement between the user who
## downloaded the software, his/her employer (which must be your employer) and
## Texas Instruments Incorporated (the License). You may not use this Software
## unless you agree to abide by the terms of the License. The License limits your
## use, and you acknowledge, that the Software may not be modified, copied or
## distributed unless embedded on a Texas Instruments microcontroller which is
## integrated into your product. Other than for the foregoing purpose, you may
## not use, reproduce, copy, prepare derivative works of, modify, distribute,
## perform, display or sell this Software and/or its documentation for any
## purpose.
## 
## YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
## PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
## INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
## NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL TEXAS
## INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
## NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL
## EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT
## LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL
## DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS,
## TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
## LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
## 
## Should you have any questions regarding your right to use this Software,
## contact Texas Instruments Incorporated at www.TI.com.
## 

## DS90Ux98x-Q1 Auto Script Generation Output
## Tool Version 2.3


import time 
import board
bridge_board_num = 2
board.setId(bridge_board_num)
## Serializer: DS90Ux983-Q1
## User Inputs:
## Serializer I2C Address= 0x18
## Max DP Lane Count = 4
## Max DP Lane Rate = 2.7Gbps
## DPRX no SSC Mode Enabled
## DP SST Mode Enabled
## DP Mode Enabled
## 983 Silicon Revision = 2.0
## SoC TX - Qualcomm 8155/6155
## FPD-Link Configuration: FPD-Link IV Dual - 6.75Gbps

## Number of Displays = 1

## Video Processor 0 (Stream 0) Properties:
## Total Horizontal Pixels = 3504
## Total Vertical Lines = 1348
## Active Horizontal Pixels = 3400
## Active Vertical Lines = 1300
## Horizontal Back Porch = 24
## Vertical Back Porch = 33
## Horizontal Sync = 32
## Vertical Sync = 1
## Horizontal Front Porch = 48
## Vertical Front Porch = 14
## Horizontal Sync Polarity = Positive
## Vertical Sync Polarity = Positive
## Bits per pixel = 30
## Pixel Clock = 283.4MHz
## PATGEN Enabled

## Deserializer 0: DS90Ux984-Q1
## User Inputs:
## Deserializer I2C Address = 0x58
## Deserializer I2C Alias = 0x58
## 984 Silicon Revision = 3.0
## FPD-Link III Input Mode

## DP Port 0 Enabled
## DP0 is outputting video 0 from the SER
## DP Port 0 PatGen Disabled
## DP Port 1 Disabled
## DP Port 1 PatGen Disabled
## DP Rate set to 2.7 Gbps
## DP lane number set to 4 lanes

## *********************************************
## Set up Variables
## *********************************************
serAddr = 0x18
desAddr0 = 0x58
desAlias0 = 0x58

board.WriteI2C( 0x18, 0x01, 0x02)  # Hard Reset

board.WriteI2C(serAddr,0x70,desAddr0)
board.WriteI2C(serAddr,0x78,desAlias0)
board.WriteI2C(serAddr,0x88,0x0)


# Programming FPDTX PLL0 = 6.750000 Gbps
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x05)
board.WriteI2C( 0x18, 0x42, 0x7D)  # ndiv=125
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x06)
board.WriteI2C( 0x18, 0x42, 0x00)  # ndiv=125
board.WriteI2C( 0x18, 0x40, 0x09)
board.WriteI2C( 0x18, 0x41, 0x04)
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x04)
board.WriteI2C( 0x18, 0x42, 0x01)  # mash order=0
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x18)
board.WriteI2C( 0x18, 0x42, 0xF6)  # denominator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x19)
board.WriteI2C( 0x18, 0x42, 0xFF)  # denominator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x1A)
board.WriteI2C( 0x18, 0x42, 0xFF)  # denominator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x1E)
board.WriteI2C( 0x18, 0x42, 0xF6)  # numerator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x1F)
board.WriteI2C( 0x18, 0x42, 0xFF)  # numerator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x20)
board.WriteI2C( 0x18, 0x42, 0xFF)  # numerator=16777206
board.WriteI2C( 0x18, 0x40, 0x09)
board.WriteI2C( 0x18, 0x41, 0x13)
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x13)
board.WriteI2C( 0x18, 0x42, 0xD0)  # post_div=1
board.WriteI2C( 0x18, 0x01, 0x30)  # FPDTX PLL0123 RESET
# Programming FPDTX PLL1 = 6.750000 Gbps
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x45)
board.WriteI2C( 0x18, 0x42, 0x7D)  # ndiv=125
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x46)
board.WriteI2C( 0x18, 0x42, 0x00)  # ndiv=125
board.WriteI2C( 0x18, 0x40, 0x09)
board.WriteI2C( 0x18, 0x41, 0x44)
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x44)
board.WriteI2C( 0x18, 0x42, 0x01)  # mash order=0
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x58)
board.WriteI2C( 0x18, 0x42, 0xF6)  # denominator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x59)
board.WriteI2C( 0x18, 0x42, 0xFF)  # denominator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x5A)
board.WriteI2C( 0x18, 0x42, 0xFF)  # denominator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x5E)
board.WriteI2C( 0x18, 0x42, 0xF6)  # numerator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x5F)
board.WriteI2C( 0x18, 0x42, 0xFF)  # numerator=16777206
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x60)
board.WriteI2C( 0x18, 0x42, 0xFF)  # numerator=16777206
board.WriteI2C( 0x18, 0x40, 0x09)
board.WriteI2C( 0x18, 0x41, 0x53)
board.WriteI2C( 0x18, 0x40, 0x08)
board.WriteI2C( 0x18, 0x41, 0x53)
board.WriteI2C( 0x18, 0x42, 0xD0)  # post_div=1
board.WriteI2C( 0x18, 0x01, 0x30)  # FPDTX PLL0123 RESET
## *********************************************
## Zero out PLL fractional - This section can be commented out to improve bringup time if 983/981 MODE_SEL0 and MODE_SEL2 are strapped to the correct FPD IV speed
## *********************************************
board.WriteI2C(serAddr,0x41,0x4)
board.WriteI2C(serAddr,0x42,0x1)
board.WriteI2C(serAddr,0x41,0x1e)
board.WriteI2C(serAddr,0x42,0x0)
board.WriteI2C(serAddr,0x41,0x1f)
board.WriteI2C(serAddr,0x42,0x0)
board.WriteI2C(serAddr,0x41,0x20)
board.WriteI2C(serAddr,0x42,0x0)
board.WriteI2C(serAddr,0x41,0x44)
board.WriteI2C(serAddr,0x42,0x1)
board.WriteI2C(serAddr,0x41,0x5e)
board.WriteI2C(serAddr,0x42,0x0)
board.WriteI2C(serAddr,0x41,0x5f)
board.WriteI2C(serAddr,0x42,0x0)
board.WriteI2C(serAddr,0x41,0x60)
board.WriteI2C(serAddr,0x42,0x0)
## *********************************************
## Configure and Enable PLLs - This section can be commented out to improve bringup time if 983/981 MODE_SEL0 and MODE_SEL2 are strapped to the correct FPD IV speed
## *********************************************
board.WriteI2C(serAddr,0x41,0xe) #Select VCO reg
board.WriteI2C(serAddr,0x42,0xc7) #Set VCO
board.WriteI2C(serAddr,0x41,0x4e) #Select VCO reg
board.WriteI2C(serAddr,0x42,0xc7) #Set VCO
board.WriteI2C(serAddr,0x1,0x30) #soft reset PLL
board.WriteI2C(serAddr,0x40,0x8) #Select PLL page
board.WriteI2C(serAddr,0x41,0x1b)
board.WriteI2C(serAddr,0x42,0x0) #Enable PLL0
board.WriteI2C(serAddr,0x41,0x5b)
board.WriteI2C(serAddr,0x42,0x0) #Enable PLL1
board.WriteI2C(serAddr,0x1,0x1) #soft reset Ser
## *********************************************
## Enable I2C Passthrough
## *********************************************
I2C_PASS_THROUGH = board.ReadI2C(serAddr,0x7,1)
I2C_PASS_THROUGH_MASK = 0x08
I2C_PASS_THROUGH_REG = I2C_PASS_THROUGH | I2C_PASS_THROUGH_MASK
board.WriteI2C(serAddr,0x07,I2C_PASS_THROUGH_REG) #Enable I2C Passthrough


## *********************************************
## Set DP Config
## *********************************************
board.WriteI2C(serAddr,0x48,0x1) #Enable APB Interface

board.WriteI2C(serAddr,0x49,0x0) #Force HPD low to configure 983 DP settings
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x0)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

board.WriteI2C(serAddr,0x49,0x74) #Set max advertised link rate = 2.7Gbps
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0xa)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

board.WriteI2C(serAddr,0x49,0x70) #Set max advertised lane count = 4
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x4)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

board.WriteI2C(serAddr,0x49,0x14) #Request min VOD swing of 0x02
board.WriteI2C(serAddr,0x4a,0x2)
board.WriteI2C(serAddr,0x4b,0x2)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

board.WriteI2C(serAddr,0x49,0x18) #Set SST/MST mode and DP/eDP Mode
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x14)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

""" board.WriteI2C(serAddr,0x49,0xc) #Disable line reset for VS0
board.WriteI2C(serAddr,0x4a,0xa)
board.WriteI2C(serAddr,0x4b,0x1)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0) """

board.WriteI2C(serAddr,0x49,0x0) #Force HPD high to trigger link training
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x1)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

#time.sleep(0.5) # Allow time after HPD is pulled high for the source to train and provide video (may need to adjust based on source properties)

## *********************************************
## Program VP Configs
## *********************************************
# Configure VP 0

board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x10)
board.WriteI2C( 0x18, 0x42, 0x48)  # Write to VID_H_ACTIVE0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x11)
board.WriteI2C( 0x18, 0x42, 0x0D)  # Write to VID_H_ACTIVE0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x12)
board.WriteI2C( 0x18, 0x42, 0x18)  # Write to VID_H_BACK0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x13)
board.WriteI2C( 0x18, 0x42, 0x00)  # Write to VID_H_BACK0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x14)
board.WriteI2C( 0x18, 0x42, 0x20)  # Write to VID_H_WIDTH0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x15)
board.WriteI2C( 0x18, 0x42, 0x00)  # Write to VID_H_WIDTH0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x16)
board.WriteI2C( 0x18, 0x42, 0xB0)  # Write to VID_H_TOTAL0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x17)
board.WriteI2C( 0x18, 0x42, 0x0D)  # Write to VID_H_TOTAL0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x18)
board.WriteI2C( 0x18, 0x42, 0x14)  # Write to VID_V_ACTIVE0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x19)
board.WriteI2C( 0x18, 0x42, 0x05)  # Write to VID_V_ACTIVE0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x1A)
board.WriteI2C( 0x18, 0x42, 0x21)  # Write to VID_V_BACK0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x1B)
board.WriteI2C( 0x18, 0x42, 0x00)  # Write to VID_V_BACK0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x1C)
board.WriteI2C( 0x18, 0x42, 0x01)  # Write to VID_V_WIDTH0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x1D)
board.WriteI2C( 0x18, 0x42, 0x00)  # Write to VID_V_WIDTH0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x1E)
board.WriteI2C( 0x18, 0x42, 0x0E)  # Write to VID_V_FRONT0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x1F)
board.WriteI2C( 0x18, 0x42, 0x00)  # Write to VID_V_FRONT0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x02)
board.WriteI2C( 0x18, 0x42, 0x48)  # Write to DP_H_ACTIVE0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x03)
board.WriteI2C( 0x18, 0x42, 0x0D)  # Write to DP_H_ACTIVE0/1_VP0
# PCLK=283.400000 MHz m=13758, n=15
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x23)
board.WriteI2C( 0x18, 0x42, 0xBE)  # Write to PCLK_GEN_M_0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x31)
board.WriteI2C( 0x18, 0x41, 0x24)
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x24)
board.WriteI2C( 0x18, 0x42, 0x35)  # Write to PCLK_GEN_M_0/1_VP0
board.WriteI2C( 0x18, 0x40, 0x31)
board.WriteI2C( 0x18, 0x41, 0x25)
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x25)
board.WriteI2C( 0x18, 0x42, 0x0F)  # Write to PCLK_GEN_N_0/1_VP0
board.WriteI2C( 0x18, 0x44, 0x01)  # Write bit VP0_ENABLE to VP_ENABLE_REG
board.WriteI2C( 0x18, 0x40, 0x31)
board.WriteI2C( 0x18, 0x41, 0x01)
board.WriteI2C( 0x18, 0x40, 0x30)
board.WriteI2C( 0x18, 0x41, 0x01)
board.WriteI2C( 0x18, 0x42, 0xA8)  # Select VP Source 0
board.WriteI2C( 0x18, 0x43, 0x00)  # Write bit NUM_VID_STREAMS to VP_CONFIG_REG
time.sleep(0.1)



## *********************************************
## Enable PATGEN
## *********************************************
board.WriteI2C(serAddr,0x40,0x30)
board.WriteI2C(serAddr,0x41,0x29)
board.WriteI2C(serAddr,0x42,0x10) #Set PATGEN Color Depth to 30bpp for VP0
board.WriteI2C(serAddr,0x41,0x28)
board.WriteI2C(serAddr,0x42,0x95) #Enable PATGEN on VP0 - Comment out this line to disable PATGEN and enable end to end video

# board.WriteI2C(serAddr,0x1,0x30) #Reset PLLs


## *********************************************
## Hold Des DTG in reset
## *********************************************
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x32)
board.WriteI2C(desAlias0,0x42,0x6) #Hold Port 0 DTG in reset
board.WriteI2C(desAlias0,0x41,0x62)
board.WriteI2C(desAlias0,0x42,0x6) #Hold Port 1 DTG in reset
board.WriteI2C( 0x58, 0x0E, 0x01)  # Set read from Port0


## *********************************************
## Setup DTG for port 0
## *********************************************
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x20)
board.WriteI2C(desAlias0,0x42,0xa3) #Set up DTG BPP, Sync Polarities, and Measurement Type
board.WriteI2C(desAlias0,0x41,0x29) #Set Hstart
board.WriteI2C(desAlias0,0x42,0x80) #Hstart upper byte
board.WriteI2C(desAlias0,0x41,0x2a)
board.WriteI2C(desAlias0,0x42,0x38) #Hstart lower byte
board.WriteI2C(desAlias0,0x41,0x2f) #Set HSW
board.WriteI2C(desAlias0,0x42,0x40) #HSW upper byte
board.WriteI2C(desAlias0,0x41,0x30)
board.WriteI2C(desAlias0,0x42,0x20) #HSW lower byte
board.WriteI2C( 0x58, 0x0E, 0x01)


## *********************************************
## Force DP Rate
## *********************************************
board.WriteI2C(desAlias0,0x40,0x2c) #Select DP Page
board.WriteI2C(desAlias0,0x41,0x81)
board.WriteI2C(desAlias0,0x42,0x60) #Set DP Rate to 2.7Gbps
board.WriteI2C(desAlias0,0x41,0x82)
board.WriteI2C(desAlias0,0x42,0x2) #Enable force DP rate
board.WriteI2C(desAlias0,0x41,0x91)
board.WriteI2C(desAlias0,0x42,0xc) #Force 4 lanes


time.sleep(0.2)
## *********************************************
## Program quad pixel clock for DP port 0
## *********************************************
board.WriteI2C(desAlias0,0xe,0x1) #Select Port0 registers
board.WriteI2C(desAlias0,0xb1,0x1) #Enable clock divider
board.WriteI2C(desAlias0,0xb2,0x8) #Program M value lower byte
board.WriteI2C(desAlias0,0xb3,0x53) #Program M value middle byte
board.WriteI2C(desAlias0,0xb4,0x4) #Program M value upper byte
board.WriteI2C(desAlias0,0xb5,0xc0) #Program N value lower byte
board.WriteI2C(desAlias0,0xb6,0x7a) #Program N value middle byte
board.WriteI2C(desAlias0,0xb7,0x10) #Program N value upper byte
# board.WriteI2C(desAlias0,0xe,0x1) #Select Port 0 registers

## *********************************************
## Release Des DTG reset
## *********************************************
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x32)
board.WriteI2C(desAlias0,0x42,0x4) #Release Port 0 DTG


## *********************************************
## Program DPTX for DP port 0
## *********************************************
# board.WriteI2C(desAlias0,0x48,0x1) #Enable APB interface

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xa4) #Set bit per color
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x40)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xb8) #Set pixel width
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x4)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xac) #Set DP Mvid
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x5a)
board.WriteI2C(desAlias0,0x4c,0x86)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xb4) #Set DP Nvid
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x0)
board.WriteI2C(desAlias0,0x4c,0x80)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xb0) #Set TU Size
board.WriteI2C(desAlias0,0x4a,0x01)
board.WriteI2C(desAlias0,0x4b,0x40)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x3e)
board.WriteI2C(desAlias0,0x4e,0x0f)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xc8) #Set TU Mode
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x0)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)


board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xc8) #Set FIFO Size
board.WriteI2C(desAlias0,0x4a,0x0)
board.WriteI2C(desAlias0,0x4b,0x10)
board.WriteI2C(desAlias0,0x4c,0x10)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xbc) #Set data count
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x74)
board.WriteI2C(desAlias0,0x4c,0xc)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)

board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0xc0) #Disable STREAM INTERLACED
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x0)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)


board.WriteI2C( 0x58, 0x48, 0x01)
board.WriteI2C( 0x58, 0x49, 0xC4)
board.WriteI2C( 0x58, 0x4A, 0x01)
board.WriteI2C( 0x58, 0x4B, 0x0F)
board.WriteI2C( 0x58, 0x4C, 0x00)
board.WriteI2C( 0x58, 0x4D, 0x00)
board.WriteI2C( 0x58, 0x4E, 0x00)


## *********************************************
## Enable DP 0 output
## *********************************************
board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(desAlias0,0x49,0x84) #Enable DP output
board.WriteI2C(desAlias0,0x4a,0x0)
board.WriteI2C(desAlias0,0x4b,0x1)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)



board.WriteI2C( 0x18, 0x40, 0x2C)
board.WriteI2C( 0x18, 0x41, 0x06)
board.WriteI2C( 0x18, 0x42, 0x40)  # Write to LINK0_SLOT_REQ0
board.WriteI2C( 0x18, 0x41, 0x01)
board.WriteI2C( 0x18, 0x42, 0x01)  # Write bit LINK0_STREAM_EN0 to LINK0_STREAM_EN
board.WriteI2C( 0x18, 0x41, 0x02)
board.WriteI2C( 0x18, 0x42, 0x10)  # Write bits LINK0_STREAM_MAP0to LINK0_MAP_REG0.0
board.WriteI2C( 0x18, 0x41, 0x00)
board.WriteI2C( 0x18, 0x42, 0x01)  # Set Link Layer 0: True
board.WriteI2C( 0x18, 0x41, 0x00)
board.WriteI2C( 0x18, 0x42, 0x03)  # Enable new Time-slot assignments for Link Layer 0
##*******************************
## TP begin
##*******************************
board.WriteI2C(serAddr,0x70,0x24)
board.WriteI2C(serAddr,0x78,0x24)
board.WriteI2C(serAddr,0x17,0x80)
board.WriteI2C(serAddr,0x18,0x81)
board.WriteI2C(serAddr,0x07,0x98)
board.WriteI2C(serAddr,0x71,0xE0)
board.WriteI2C(serAddr,0x79,0xE0)
##*******************************
##interrupt GPIO6 TI983 FOR UNLOCK
##*******************************
board.WriteI2C(serAddr,0x1D,0x89)

board.endI2C()