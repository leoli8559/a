## TI Confidential - NDA Restrictions
## 
## Copyright 2018 Texas Instruments Incorporated. All rights reserved.
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
## Tool Version 2.0


import time 
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
## FPD-Link Configuration: FPD-Link IV Single Port 0 - 6.75Gbps

## Number of Displays = 1

## Video Processor 0 (Stream 0) Properties:
## Total Horizontal Pixels = 2088
## Total Vertical Lines = 940
## Active Horizontal Pixels = 2000
## Active Vertical Lines = 810
## Horizontal Back Porch = 16
## Vertical Back Porch = 30
## Horizontal Sync = 16
## Vertical Sync = 50
## Horizontal Front Porch = 56
## Vertical Front Porch = 50
## Horizontal Sync Polarity = Positive
## Vertical Sync Polarity = Positive
## Bits per pixel = 30
## Pixel Clock = 117.8MHz
## PATGEN Enabled

## Deserializer 0: DS90Ux984-Q1
## Note: The 984 CS3 should be strapped to the desired FPD link rate setting based on the intended configuration
## Note: The 984 CS3 should be strapped to 6.75Gbps FPD IV mode using MODE_SEL0
## User Inputs:
## Deserializer I2C Address = 0x58
## Deserializer I2C Alias = 0x58
## 984 Silicon Revision = 3.0
## FPD-Link IV Input Rate 6.75Gbps
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
board.WriteI2C(serAddr,0x70,0x58)
board.WriteI2C(serAddr,0x78,0x58)
board.WriteI2C(serAddr,0x88,0x0)
## *********************************************
## Program SER to FPD-Link IV mode
## *********************************************
board.WriteI2C(serAddr,0x40,0x8) #Select PLL reg page
board.WriteI2C(serAddr,0x41,0x1b)
board.WriteI2C(serAddr,0x42,0x8) #Disable PLL0
board.WriteI2C(serAddr,0x41,0x5b)
board.WriteI2C(serAddr,0x42,0x8) #Disable PLL1
board.WriteI2C(serAddr,0x5b,0x23) #Disable FPD3 FIFO pass through
board.WriteI2C(serAddr,0x59,0x5) #Change FPDTX FPD3_MODE_CTL to independent
board.WriteI2C(serAddr,0x5,0x3c) #Force FPD4_TX independent mode
board.WriteI2C(serAddr,0x2,0xd1) #Enable mode overwrite
board.WriteI2C(serAddr,0x2d,0x1)
board.WriteI2C(serAddr,0x70,0x24)
board.WriteI2C(serAddr,0x78,0x24)
board.WriteI2C(serAddr,0x40,0x24) #Select digital reg page
board.WriteI2C(serAddr,0x41,0x84)
board.WriteI2C(serAddr,0x42,0x2) #Switch encoder from FPD3 to FPD4
board.WriteI2C(serAddr,0x41,0x94)
board.WriteI2C(serAddr,0x42,0x2) #Switch encoder from FPD3 to FPD4
board.WriteI2C(serAddr,0x40,0x8) #Select PLL page
board.WriteI2C(serAddr,0x41,0x5) #Select Ncount Reg
board.WriteI2C(serAddr,0x42,0x7d) #Set Ncount
board.WriteI2C(serAddr,0x41,0x13) #Select post div reg
board.WriteI2C(serAddr,0x42,0x90) #Set post div for 6.75 Gbps
board.WriteI2C(serAddr,0x41,0x45) #Select Ncount Reg
board.WriteI2C(serAddr,0x42,0x7d) #Set Ncount
board.WriteI2C(serAddr,0x41,0x53) #Select post div reg
board.WriteI2C(serAddr,0x42,0x90) #Set post div for 6.75 Gbps
board.WriteI2C(serAddr,0x2d,0x3) #Select write reg to both ports
board.WriteI2C(serAddr,0x6a,0xa) #set BC sampling rate
board.WriteI2C(serAddr,0x6e,0x80) #set BC fractional sampling
## *********************************************
## Zero out PLL fractional
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
## Configure and Enable PLLs
## *********************************************
board.WriteI2C(serAddr,0x41,0xe) #Select VCO reg
board.WriteI2C(serAddr,0x42,0xc7) #Set VCO
board.WriteI2C(serAddr,0x41,0x4e) #Select VCO reg
board.WriteI2C(serAddr,0x42,0xc7) #Set VCO
board.WriteI2C(serAddr,0x1,0x30) #soft reset PLL
time.sleep(1)
board.WriteI2C(serAddr,0x40,0x8) #Select PLL page
board.WriteI2C(serAddr,0x41,0x1b)
board.WriteI2C(serAddr,0x42,0x0) #Enable PLL0
board.WriteI2C(serAddr,0x1,0x1) #soft reset Ser
time.sleep(1)
## *********************************************
## Enable I2C Passthrough
## *********************************************
I2C_PASS_THROUGH = board.ReadI2C(serAddr,0x7,1)
I2C_PASS_THROUGH_MASK = 0x08
I2C_PASS_THROUGH_REG = I2C_PASS_THROUGH | I2C_PASS_THROUGH_MASK
board.WriteI2C(serAddr,0x07,I2C_PASS_THROUGH_REG) #Enable I2C Passthrough

board.WriteI2C(desAlias0,0x1,0x1) #Soft reset Des
time.sleep(1)
board.WriteI2C(serAddr,0x40,0x2e) #Select Ser link layer
board.WriteI2C(serAddr,0x41,0x0)
board.WriteI2C(serAddr,0x42,0x3) #refresh time slot
board.WriteI2C(serAddr,0x2d,0x1) #Select write to port0 reg
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

board.WriteI2C(serAddr,0x49,0xc) #Disable line reset for VS0
board.WriteI2C(serAddr,0x4a,0xa)
board.WriteI2C(serAddr,0x4b,0x1)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

board.WriteI2C(serAddr,0x49,0x0) #Force HPD high to trigger link training
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x1)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

time.sleep(0.5) # Allow time after HPD is pulled high for the source to train and provide video (may need to adjust based on source properties)

## *********************************************
## Program VP Configs
## *********************************************
# Configure VP 0
board.WriteI2C(serAddr,0x40,0x32)
board.WriteI2C(serAddr,0x41,0x1)
board.WriteI2C(serAddr,0x42,0xa8) #Set VP_SRC_SELECT to Stream 0 for SST Mode
board.WriteI2C(serAddr,0x41,0x2)
board.WriteI2C(serAddr,0x42,0xd0) #VID H Active
board.WriteI2C(serAddr,0x42,0x7) #VID H Active
board.WriteI2C(serAddr,0x41,0x10)
board.WriteI2C(serAddr,0x42,0xd0) #Horizontal Active
board.WriteI2C(serAddr,0x42,0x7) #Horizontal Active
board.WriteI2C(serAddr,0x42,0x10) #Horizontal Back Porch
board.WriteI2C(serAddr,0x42,0x0) #Horizontal Back Porch
board.WriteI2C(serAddr,0x42,0x10) #Horizontal Sync
board.WriteI2C(serAddr,0x42,0x0) #Horizontal Sync
board.WriteI2C(serAddr,0x42,0x28) #Horizontal Total
board.WriteI2C(serAddr,0x42,0x8) #Horizontal Total
board.WriteI2C(serAddr,0x42,0x2a) #Vertical Active
board.WriteI2C(serAddr,0x42,0x3) #Vertical Active
board.WriteI2C(serAddr,0x42,0x1e) #Vertical Back Porch
board.WriteI2C(serAddr,0x42,0x0) #Vertical Back Porch
board.WriteI2C(serAddr,0x42,0x32) #Vertical Sync
board.WriteI2C(serAddr,0x42,0x0) #Vertical Sync
board.WriteI2C(serAddr,0x42,0x32) #Vertical Front Porch
board.WriteI2C(serAddr,0x42,0x0) #Vertical Front Porch
board.WriteI2C(serAddr,0x41,0x27)
board.WriteI2C(serAddr,0x42,0x0) #HSYNC Polarity = +, VSYNC Polarity = +
board.WriteI2C(serAddr,0x41,0x23) #M/N Register
board.WriteI2C(serAddr,0x42,0x57) #M value
board.WriteI2C(serAddr,0x42,0x16) #M value
board.WriteI2C(serAddr,0x42,0xf) #N value


## *********************************************
## Enable VPs
## *********************************************
board.WriteI2C(serAddr,0x43,0x0) #Set number of VPs used = 1
board.WriteI2C(serAddr,0x44,0x1) #Enable video processors

## *********************************************
## Issue DP video reset after video is available from the DP source
## *********************************************
board.WriteI2C(serAddr,0x49,0x0) #Read back Horizontal Resolution from DP APB
board.WriteI2C(serAddr,0x4a,0x5)
board.WriteI2C(serAddr,0x48,0x3)
apbData0 = board.ReadI2C(serAddr,0x4b,1)
apbData1 = board.ReadI2C(serAddr,0x4c,1)
apbData2 = board.ReadI2C(serAddr,0x4d,1)
apbData3 = board.ReadI2C(serAddr,0x4e,1)
apbData = (apbData3<<24) | (apbData2<<16) | (apbData1<<8) | (apbData0<<0)
HRes = apbData

board.WriteI2C(serAddr,0x49,0x14) #Read back Vertical Resolution from DP APB
board.WriteI2C(serAddr,0x4a,0x5)
board.WriteI2C(serAddr,0x48,0x3)
apbData0 = board.ReadI2C(serAddr,0x4b,1)
apbData1 = board.ReadI2C(serAddr,0x4c,1)
apbData2 = board.ReadI2C(serAddr,0x4d,1)
apbData3 = board.ReadI2C(serAddr,0x4e,1)
apbData = (apbData3<<24) | (apbData2<<16) | (apbData1<<8) | (apbData0<<0)
VRes = apbData

print "Detected DP Input Resolution: ", HRes, "x", VRes

if HRes == 0 or VRes == 0:
  print "Warning, no DP Video Input to 983 Detected - try adding more delay after HPD is pulled high in the Set DP Config Section"

board.WriteI2C(serAddr,0x49,0x54) #Video Input Reset (should be executed after DP video is available from the source)
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x1)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)

## *********************************************
## Enable PATGEN
## *********************************************
board.WriteI2C(serAddr,0x40,0x30)
board.WriteI2C(serAddr,0x41,0x29)
board.WriteI2C(serAddr,0x42,0x10) #Set PATGEN Color Depth to 30bpp for VP0
board.WriteI2C(serAddr,0x41,0x28)
board.WriteI2C(serAddr,0x42,0x95) #Enable PATGEN on VP0 - Comment out this line to disable PATGEN and enable end to end video

board.WriteI2C(serAddr,0x1,0x30) #Reset PLLs

## *********************************************
## Configure Serializer TX Link Layer
## *********************************************
board.WriteI2C(serAddr,0x40,0x2e) #Link layer Reg page
board.WriteI2C(serAddr,0x41,0x1) #Link layer stream enable
board.WriteI2C(serAddr,0x42,0x1) #Link layer stream enable
board.WriteI2C(serAddr,0x41,0x6) #Link layer time slot 0
board.WriteI2C(serAddr,0x42,0x3c) #Link layer time slot
board.WriteI2C(serAddr,0x41,0x20) #Set Link layer vp bpp
board.WriteI2C(serAddr,0x42,0x5a) #Set Link layer vp bpp according to VP Bit per pixel
board.WriteI2C(serAddr,0x41,0x0) #Link layer enable
board.WriteI2C(serAddr,0x42,0x3) #Link layer enable

## *********************************************
## Read Deserializer 0 Temp
## *********************************************
board.WriteI2C(desAlias0,0x40,0x6c)
board.WriteI2C(desAlias0,0x41,0xd)
board.WriteI2C(desAlias0,0x42,0x0)
board.WriteI2C(desAlias0,0x41,0x13)
TEMP_FINAL = board.ReadI2C(desAlias0,0x42,1)
TEMP_FINAL_C = 2*TEMP_FINAL - 273

## *********************************************
## Set up Deserializer 0 Temp Ramp Optimizations
## *********************************************
Ramp_UP_Range_CODES_Needed = int((150-TEMP_FINAL_C)/(190/11)) + 1
Ramp_DN_Range_CODES_Needed = int((TEMP_FINAL_C-30)/(190/11)) + 1
Ramp_UP_CAP_DELTA = Ramp_UP_Range_CODES_Needed - 4
Ramp_DN_CAP_DELTA = Ramp_DN_Range_CODES_Needed - 7

if Ramp_UP_CAP_DELTA > 0:
  TS_CODE_UP = 4 - Ramp_UP_CAP_DELTA
  if TS_CODE_UP < 0:
      TS_CODE_UP = 0
  board.WriteI2C(desAlias0,0x40,0x3c)
  board.WriteI2C(desAlias0,0x41,0xf5)
  rb = board.ReadI2C(desAlias0,0x42,1)
  rb &= 0x8F
  rb |= (TS_CODE_UP << 4)
  board.WriteI2C(desAlias0,0x42,rb)
  rb = board.ReadI2C(desAlias0,0x42,1)
  rb &= 0xFE
  rb |= 0x01
  board.WriteI2C(desAlias0,0x42,rb)
  board.WriteI2C(desAlias0,0x1,0x1)

if Ramp_DN_CAP_DELTA > 0:
  TS_CODE_DN = 4 + Ramp_DN_CAP_DELTA
  if TS_CODE_DN >= 7:
      TS_CODE_DN = 7
  board.WriteI2C(desAlias0,0x40,0x3c)
  board.WriteI2C(desAlias0,0x41,0xf5)
  rb = board.ReadI2C(desAlias0,0x42,1)
  rb &= 0x8F
  rb |= (TS_CODE_DN << 4)
  board.WriteI2C(desAlias0,0x42,rb)
  rb = board.ReadI2C(desAlias0,0x42,1)
  rb &= 0xFE
  rb |= 0x01
  board.WriteI2C(desAlias0,0x42,rb)
  board.WriteI2C(desAlias0,0x1,0x1)

## *********************************************
## Clear CRC errors from initial link process
## *********************************************

Reg_value = board.ReadI2C(serAddr,0x2,1)
Reg_value = Reg_value | 0x20
board.WriteI2C(serAddr,0x2,Reg_value) #CRC Error Reset

Reg_value = board.ReadI2C(serAddr,0x2,1)
Reg_value = Reg_value & 0xdf
board.WriteI2C(serAddr,0x2,Reg_value) #CRC Error Reset Clear

board.WriteI2C(serAddr,0x2d,0x1)

## *********************************************
## Hold Des DTG in reset
## *********************************************
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x32)
board.WriteI2C(desAlias0,0x42,0x2) #Hold Port 0 DTG in reset
board.WriteI2C(desAlias0,0x41,0x62)
board.WriteI2C(desAlias0,0x42,0x2) #Hold Port 1 DTG in reset


## *********************************************
## Disable Stream Mapping
## *********************************************
board.WriteI2C(desAlias0,0xe,0x3) #Select both Output Ports
board.WriteI2C(desAlias0,0xd0,0x0) #Disable FPD4 video forward to Output Port
board.WriteI2C(desAlias0,0xd7,0x0) #Disable FPD3 video forward to Output Port


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
board.WriteI2C(desAlias0,0x40,0x30) #Disable DP SSCG
board.WriteI2C(desAlias0,0x41,0xf)
board.WriteI2C(desAlias0,0x42,0x1)
board.WriteI2C(desAlias0,0x1,0x40)


## *********************************************
## Setup DP ports
## *********************************************
board.WriteI2C(desAlias0,0xe,0x12) #Select Port 1 registers
board.WriteI2C(desAlias0,0x46,0x0) #Disable DP Port 1
board.WriteI2C(desAlias0,0xe,0x1) #Select Port 0 registers
board.WriteI2C(desAlias0,0x1,0x40) #DP-TX-PLL RESET Applied


## *********************************************
## Map video to display output
## *********************************************
board.WriteI2C(desAlias0,0xe,0x3) #Select both Output Ports
board.WriteI2C(desAlias0,0xd0,0xc) #Enable FPD_RX video forward to Output Port
board.WriteI2C(desAlias0,0xd1,0xf) #Every stream forwarded on DC
board.WriteI2C(desAlias0,0xd6,0x8) #Send Stream 0 to Output Port 0 and Send Stream 1 to Output Port 1
board.WriteI2C(desAlias0,0xd7,0x0) #FPD3 mapping disabled
board.WriteI2C(desAlias0,0xe,0x1) #Select Port 0 


## *********************************************
## Program quad pixel clock for DP port 0
## *********************************************
board.WriteI2C(desAlias0,0xe,0x1) #Select Port0 registers
board.WriteI2C(desAlias0,0xb1,0x1) #Enable clock divider
board.WriteI2C(desAlias0,0xb2,0x28) #Program M value lower byte
board.WriteI2C(desAlias0,0xb3,0xcc) #Program M value middle byte
board.WriteI2C(desAlias0,0xb4,0x1) #Program M value upper byte
board.WriteI2C(desAlias0,0xb5,0xc0) #Program N value lower byte
board.WriteI2C(desAlias0,0xb6,0x7a) #Program N value middle byte
board.WriteI2C(desAlias0,0xb7,0x10) #Program N value upper byte
board.WriteI2C(desAlias0,0xe,0x1) #Select Port 0 registers


## *********************************************
## Setup DTG for port 0
## *********************************************
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x20)
board.WriteI2C(desAlias0,0x42,0xa3) #DTG detect HS active high and VS active high
board.WriteI2C(desAlias0,0x41,0x29) #Set Hstart
board.WriteI2C(desAlias0,0x42,0x80) #Hstart upper byte
board.WriteI2C(desAlias0,0x41,0x2a)
board.WriteI2C(desAlias0,0x42,0x20) #Hstart lower byte
board.WriteI2C(desAlias0,0x41,0x2f) #Set HSW
board.WriteI2C(desAlias0,0x42,0x40) #HSW upper byte
board.WriteI2C(desAlias0,0x41,0x30)
board.WriteI2C(desAlias0,0x42,0x10) #HSW lower byte


## *********************************************
## Program DPTX for DP port 0
## *********************************************
board.WriteI2C(desAlias0,0x48,0x1) #Enable APB interface
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
board.WriteI2C(desAlias0,0x4b,0xd8)
board.WriteI2C(desAlias0,0x4c,0x37)
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

board.WriteI2C(desAlias0,0x49,0xc8) #Set TU Mode
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x0)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)

board.WriteI2C(desAlias0,0x49,0xb0) #Set TU Size
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x40)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x1a)
board.WriteI2C(desAlias0,0x4e,0x4)
board.WriteI2C(desAlias0,0x48,0x1)

board.WriteI2C(desAlias0,0x49,0xc8) #Set FIFO Size
board.WriteI2C(desAlias0,0x4a,0x0)
board.WriteI2C(desAlias0,0x4b,0x6)
board.WriteI2C(desAlias0,0x4c,0x40)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)

board.WriteI2C(desAlias0,0x49,0xbc) #Set data count
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x53)
board.WriteI2C(desAlias0,0x4c,0x7)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)

board.WriteI2C(desAlias0,0x49,0xc0) #Disable STREAM INTERLACED
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x0)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)

board.WriteI2C(desAlias0,0x49,0xc4) #Set SYNC polarity
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0xc)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)



## *********************************************
## Release Des DTG reset
## *********************************************
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x32)
board.WriteI2C(desAlias0,0x42,0x0) #Release Des DTG reset
board.WriteI2C(desAlias0,0x41,0x62)
board.WriteI2C(desAlias0,0x42,0x0) #Release Des DTG reset

board.WriteI2C(desAlias0,0x49,0x80) #Set Htotal
board.WriteI2C(desAlias0,0x4a,0x1)
board.WriteI2C(desAlias0,0x4b,0x28)
board.WriteI2C(desAlias0,0x4c,0x8)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)

## *********************************************
## Enable DP 0 output
## *********************************************
board.WriteI2C(desAlias0,0x49,0x84) #Enable DP output
board.WriteI2C(desAlias0,0x4a,0x0)
board.WriteI2C(desAlias0,0x4b,0x1)
board.WriteI2C(desAlias0,0x4c,0x0)
board.WriteI2C(desAlias0,0x4d,0x0)
board.WriteI2C(desAlias0,0x4e,0x0)
board.WriteI2C(desAlias0,0x48,0x1)
board.WriteI2C(serAddr,0x1D,0x89)

# Paste at end of existing script

FIFO_OV = board.ReadAPBI2C(desAlias0,0x1CC)
if FIFO_OV > 0:
        board.WriteI2C(desAlias0,0x1,0x1)

measure = [0,0]
board.WriteI2C(desAlias0,0x40,0x53)
board.WriteI2C(desAlias0,0x41,0x42)
for x in range(0,2,1):
        measure[x] = board.ReadI2C(desAlias0,0x42,1)
Vtotal = (measure[0] << 8) | measure[1]

if Vtotal != 940:
        board.WriteI2C(desAlias0,0x1,0x1)