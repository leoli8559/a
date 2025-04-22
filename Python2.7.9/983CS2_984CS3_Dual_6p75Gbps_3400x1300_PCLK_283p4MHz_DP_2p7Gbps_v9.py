## TI Confidential - NDA Restrictions
## 
## Copyright 2022 Texas Instruments Incorporated. All rights reserved.
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
## Tool Version 2.4


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
## Note: The 984 CS3 should be strapped to 6.75Gbps FPD IV mode using MODE_SEL0
## User Inputs:
## Deserializer I2C Address = 0x58
## Deserializer I2C Alias = 0x58
## 984 Silicon Revision = 3.0
## DP Port 0 Enabled
## DP0 is outputting video 0 from the SER
## DP Port 0 PatGen Disabled
## DP Port 1 Disabled
## DP Port 1 PatGen Disabled
## DP Rate set to 2.7 Gbps
## DP lane number set to 4 lanes

def lower_byte(in_data):
	out_data= (in_data & 0xFF)
	return out_data

def upper_byte(in_data):
	out_data=  (in_data & 0xFF00)>>8
	return out_data

def upper_byte_24(in_data):
	out_data=  (in_data & 0xFF0000)>>16
	return out_data

serAddr = 0x18
desAddr0 = 0x58
desAlias0 = 0x58

def sweepPCLK(desPCLK=282.96,serPCLK=282.96):
	## *********************************************
	## Set up Variables
	## *********************************************
	board.WriteI2C(serAddr,0x01,0x02)  # Hard Reset
	board.WriteI2C(desAlias0,0x01,0x02)  # Hard Reset

	print "------------------------------"
	print "desPCLK: ", desPCLK
	print "serPCLK: ", serPCLK
	board.WriteI2C(serAddr,0x70,desAddr0)
	board.WriteI2C(serAddr,0x78,desAlias0)
	board.WriteI2C(serAddr,0x88,0x00)

	## *********************************************
	## Program SER to FPD-Link IV mode
	## *********************************************
	board.WriteI2C(serAddr,0x5b,0x23) #Disable FPD3 FIFO pass through
	board.WriteI2C(serAddr,0x05,0x28) #Force FPD4_TX dual mode
	## *********************************************
	## Set up FPD IV PLL Settings - This section can be commented out to improve bringup time if 983/981 MODE_SEL0 and MODE_SEL2 are strapped to the correct FPD IV speed
	## *********************************************
	board.WriteI2C(serAddr,0x40,0x08) #Select PLL reg page
	board.WriteI2C(serAddr,0x41,0x1b)
	board.WriteI2C(serAddr,0x42,0x08) #Disable PLL0
	board.WriteI2C(serAddr,0x41,0x5b)
	board.WriteI2C(serAddr,0x42,0x80) #Disable PLL1
	board.WriteI2C(serAddr,0x02,0xd1) #Enable mode overwrite
	board.WriteI2C(serAddr,0x2d,0x01)
	board.WriteI2C(serAddr,0x40,0x24) #Select digital reg page
	board.WriteI2C(serAddr,0x40,0x08) #Select PLL page
	board.WriteI2C(serAddr,0x41,0x05) #Select Ncount Reg
	board.WriteI2C(serAddr,0x42,0x7d) #Set Ncount
	board.WriteI2C(serAddr,0x41,0x13) #Select post div reg
	board.WriteI2C(serAddr,0x42,0x90) #Set post div for 6.75 Gbps
	board.WriteI2C(serAddr,0x2d,0x01) #Select write reg to port 0
	board.WriteI2C(serAddr,0x6a,0x0a) #set BC sampling rate
	board.WriteI2C(serAddr,0x6e,0x80) #set BC fractional sampling
	board.WriteI2C(serAddr,0x41,0x45) #Select Ncount Reg
	board.WriteI2C(serAddr,0x42,0x7d) #Set Ncount
	board.WriteI2C(serAddr,0x41,0x53) #Select post div reg
	board.WriteI2C(serAddr,0x42,0x90) #Set post div for 6.75 Gbps
	board.WriteI2C(serAddr,0x2d,0x12) #Select write reg to port 1
	board.WriteI2C(serAddr,0x6a,0x0a) #set BC sampling rate
	board.WriteI2C(serAddr,0x6e,0x80) #set BC fractional sampling
	board.WriteI2C(serAddr,0x02,0xd1) #Set HALFRATE_MODE
	## *********************************************
	## Zero out PLL fractional - This section can be commented out to improve bringup time if 983/981 MODE_SEL0 and MODE_SEL2 are strapped to the correct FPD IV speed
	## *********************************************
	board.WriteI2C(serAddr,0x41,0x04)
	board.WriteI2C(serAddr,0x42,0x01)
	board.WriteI2C(serAddr,0x41,0x1e)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x1f)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x20)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x44)
	board.WriteI2C(serAddr,0x42,0x01)
	board.WriteI2C(serAddr,0x41,0x5e)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x5f)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x60)
	board.WriteI2C(serAddr,0x42,0x00)
	## *********************************************
	## Configure and Enable PLLs - This section can be commented out to improve bringup time if 983/981 MODE_SEL0 and MODE_SEL2 are strapped to the correct FPD IV speed
	## *********************************************
	board.WriteI2C(serAddr,0x41,0x0e) #Select VCO reg
	board.WriteI2C(serAddr,0x42,0xc7) #Set VCO
	board.WriteI2C(serAddr,0x41,0x4e) #Select VCO reg
	board.WriteI2C(serAddr,0x42,0xc7) #Set VCO
	board.WriteI2C(serAddr,0x01,0x30) #soft reset PLL
	board.WriteI2C(serAddr,0x40,0x08) #Select PLL page
	board.WriteI2C(serAddr,0x41,0x1b)
	board.WriteI2C(serAddr,0x42,0x00) #Enable PLL0
	board.WriteI2C(serAddr,0x41,0x5b)
	board.WriteI2C(serAddr,0x42,0x00) #Enable PLL1
	board.WriteI2C(serAddr,0x01,0x01) #soft reset Ser
	## *********************************************
	## Enable I2C Passthrough
	## *********************************************
	I2C_PASS_THROUGH = board.ReadI2C(serAddr,0x7,1)
	I2C_PASS_THROUGH_MASK = 0x08
	I2C_PASS_THROUGH_REG = I2C_PASS_THROUGH | I2C_PASS_THROUGH_MASK
	board.WriteI2C(serAddr,0x07,I2C_PASS_THROUGH_REG) #Enable I2C Passthrough
    #print "I2C_PASS_THROUGH_REG: ", I2C_PASS_THROUGH_REG
	board.WriteI2C(desAlias0,0x01,0x01) #Soft reset Des
	time.sleep(0.04)
	board.WriteI2C(serAddr,0x40,0x2e) #Select Ser link layer
	board.WriteI2C(serAddr,0x41,0x00)
	board.WriteI2C(serAddr,0x42,0x03) #refresh time slot
	board.WriteI2C(serAddr,0x2d,0x01) #Select write to port0 reg
	## *********************************************
	## Set DP Config
	## *********************************************
	board.WriteI2C(serAddr,0x48,0x01) #Enable APB Interface

	board.WriteI2C(serAddr,0x49,0x00) #Force HPD low to configure 983 DP settings
	board.WriteI2C(serAddr,0x4a,0x00)
	board.WriteI2C(serAddr,0x4b,0x00)
	board.WriteI2C(serAddr,0x4c,0x00)
	board.WriteI2C(serAddr,0x4d,0x00)
	board.WriteI2C(serAddr,0x4e,0x00)
	
	##CDR Settings for 2.7Gbps no SSC with 8155
	##Lane0
	board.WriteI2C(serAddr,0x40,0x10)
	board.WriteI2C(serAddr,0x41,0x2F)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x4A)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x4E)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x55)
	board.WriteI2C(serAddr,0x42,0x00)
	##Lane1
	board.WriteI2C(serAddr,0x40,0x10)
	board.WriteI2C(serAddr,0x41,0xAF)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0xCA)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0xCE)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0xD5)
	board.WriteI2C(serAddr,0x42,0x00)
	##Lane2
	board.WriteI2C(serAddr,0x40,0x14)
	board.WriteI2C(serAddr,0x41,0x2F)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x4A)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x4E)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0x55)
	board.WriteI2C(serAddr,0x42,0x00)
	##Lane3
	board.WriteI2C(serAddr,0x40,0x14)
	board.WriteI2C(serAddr,0x41,0xAF)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0xCA)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0xCE)
	board.WriteI2C(serAddr,0x42,0x00)
	board.WriteI2C(serAddr,0x41,0xD5)
	board.WriteI2C(serAddr,0x42,0x00)
	
	board.WriteI2C(serAddr,0x49,0x74) #Set max advertised link rate = 2.7Gbps
	board.WriteI2C(serAddr,0x4a,0x00)
	board.WriteI2C(serAddr,0x4b,0x0a)
	board.WriteI2C(serAddr,0x4c,0x00)
	board.WriteI2C(serAddr,0x4d,0x00)
	board.WriteI2C(serAddr,0x4e,0x00)

	board.WriteI2C(serAddr,0x49,0x70) #Set max advertised lane count = 4
	board.WriteI2C(serAddr,0x4a,0x00)
	board.WriteI2C(serAddr,0x4b,0x04)
	board.WriteI2C(serAddr,0x4c,0x00)
	board.WriteI2C(serAddr,0x4d,0x00)
	board.WriteI2C(serAddr,0x4e,0x00)

	board.WriteI2C(serAddr,0x49,0x14) #Request min VOD swing of 0x02
	board.WriteI2C(serAddr,0x4a,0x02)
	board.WriteI2C(serAddr,0x4b,0x02)
	board.WriteI2C(serAddr,0x4c,0x00)
	board.WriteI2C(serAddr,0x4d,0x00)
	board.WriteI2C(serAddr,0x4e,0x00)

	board.WriteI2C(serAddr,0x49,0x18) #Set SST/MST mode and DP/eDP Mode
	board.WriteI2C(serAddr,0x4a,0x00)
	board.WriteI2C(serAddr,0x4b,0x14)
	board.WriteI2C(serAddr,0x4c,0x00)
	board.WriteI2C(serAddr,0x4d,0x00)
	board.WriteI2C(serAddr,0x4e,0x00)

	board.WriteI2C(serAddr,0x49,0x00) #Force HPD high to trigger link training
	board.WriteI2C(serAddr,0x4a,0x00)
	board.WriteI2C(serAddr,0x4b,0x01)
	board.WriteI2C(serAddr,0x4c,0x00)
	board.WriteI2C(serAddr,0x4d,0x00)
	board.WriteI2C(serAddr,0x4e,0x00)
	

	board.WriteI2C(serAddr, 0x40, 0x30)
	board.WriteI2C(serAddr, 0x41, 0x30)
	tempread = board.ReadI2C(serAddr,0x42,1)
	bit = 0
	if tempread & (1 << bit) == 0:
		board.WriteI2C(serAddr,0x49,0x54) #Video Input Reset due to the wraparound demo video
		board.WriteI2C(serAddr,0x4a,0x00)
		board.WriteI2C(serAddr,0x4b,0x01)
		board.WriteI2C(serAddr,0x4c,0x00)
		board.WriteI2C(serAddr,0x4d,0x00)
		board.WriteI2C(serAddr,0x4e,0x00)
	
	#time.sleep(0.5) # Allow time after HPD is pulled high for the source to train and provide video (may need to adjust based on source properties)

	## *********************************************
	## Program VP Configs
	## *********************************************
	# Configure VP 0
	board.WriteI2C(serAddr,0x40,0x32)
	board.WriteI2C(serAddr,0x41,0x01)
	board.WriteI2C(serAddr,0x42,0xa8) #Set VP_SRC_SELECT to Stream 0 for SST Mode
	board.WriteI2C(serAddr,0x41,0x02)
	board.WriteI2C(serAddr,0x42,0x48) #VID H Active
	board.WriteI2C(serAddr,0x42,0x0d) #VID H Active
	board.WriteI2C(serAddr,0x41,0x10)
	board.WriteI2C(serAddr,0x42,0x48) #Horizontal Active
	board.WriteI2C(serAddr,0x42,0x0d) #Horizontal Active
	board.WriteI2C(serAddr,0x42,0x18) #Horizontal Back Porch
	board.WriteI2C(serAddr,0x42,0x00) #Horizontal Back Porch
	board.WriteI2C(serAddr,0x42,0x20) #Horizontal Sync
	board.WriteI2C(serAddr,0x42,0x00) #Horizontal Sync
	board.WriteI2C(serAddr,0x42,0xb0) #Horizontal Total
	board.WriteI2C(serAddr,0x42,0x0d) #Horizontal Total
	board.WriteI2C(serAddr,0x42,0x14) #Vertical Active
	board.WriteI2C(serAddr,0x42,0x05) #Vertical Active
	board.WriteI2C(serAddr,0x42,0x21) #Vertical Back Porch
	board.WriteI2C(serAddr,0x42,0x00) #Vertical Back Porch
	board.WriteI2C(serAddr,0x42,0x01) #Vertical Sync
	board.WriteI2C(serAddr,0x42,0x00) #Vertical Sync
	board.WriteI2C(serAddr,0x42,0x0e) #Vertical Front Porch
	board.WriteI2C(serAddr,0x42,0x00) #Vertical Front Porch
	board.WriteI2C(serAddr,0x41,0x27)
	board.WriteI2C(serAddr,0x42,0x00) #HSYNC Polarity = +, VSYNC Polarity = +
	board.WriteI2C(serAddr,0x41,0x23) #M/N Register
	m_value = int(serPCLK / 4.0 * (2 ** 15) / (6750/40.0))
	print "SER m_value: ", m_value
	print "SER n_value: 15"
	board.WriteI2C(serAddr,0x42,lower_byte(m_value)) #M value
	board.WriteI2C(serAddr,0x42,upper_byte(m_value)) #M value
	board.WriteI2C(serAddr,0x42,0x0f) #N value


	## *********************************************
	## Enable VPs
	## *********************************************
	board.WriteI2C(serAddr,0x43,0x00) #Set number of VPs used = 1
	board.WriteI2C(serAddr,0x44,0x01) #Enable video processors

	## *********************************************
	## Enable PATGEN
	## *********************************************
	board.WriteI2C(serAddr,0x40,0x30)
	board.WriteI2C(serAddr,0x41,0x29)
	board.WriteI2C(serAddr,0x42,0x10) #Set PATGEN Color Depth to 30bpp for VP0
	board.WriteI2C(serAddr,0x41,0x28)
	board.WriteI2C(serAddr,0x42,0x95) #0x5cEnable PATGEN on VP0 - Comment out this line to disable PATGEN and enable end to end video
	board.WriteI2C(serAddr,0x01,0x30) #Reset PLLs

	##*********************************************
	## Configure Serializer TX Link Layer
	##*********************************************
	board.WriteI2C(serAddr,0x40,0x2e) #Link layer Reg page
	board.WriteI2C(serAddr,0x41,0x01) #Link layer 0 stream enable
	board.WriteI2C(serAddr,0x42,0x01) #Link layer 0 stream enable
	board.WriteI2C(serAddr,0x41,0x06) #Link layer 0 time slot 0
	board.WriteI2C(serAddr,0x42,0x3c) #Link layer 0 time slot
	board.WriteI2C(serAddr,0x41,0x20) #Set Link layer vp bpp
	board.WriteI2C(serAddr,0x42,0x5a) #Set Link layer vp bpp according to VP Bit per pixel
	board.WriteI2C(serAddr,0x41,0x00) #Link layer 0 enable
	board.WriteI2C(serAddr,0x42,0x03) #Link layer 0 enable

	## *********************************************
	## Read Deserializer 0 Temp
	## *********************************************
	board.WriteI2C(desAlias0,0x40,0x6c)
	board.WriteI2C(desAlias0,0x41,0x0d)
	board.WriteI2C(desAlias0,0x42,0x00)
	board.WriteI2C(desAlias0,0x41,0x13)
	TEMP_FINAL = board.ReadI2C(desAlias0,0x42,1)
	TEMP_FINAL_C = 2*TEMP_FINAL - 273

	## *********************************************
	## Set up Deserializer 0 Temp Ramp Optimizations
	## *********************************************
	Efuse_TS_CODE = 2
	Ramp_UP_Range_CODES_Needed = int((150-TEMP_FINAL_C)/(190/11)) + 1
	Ramp_DN_Range_CODES_Needed = int((TEMP_FINAL_C-30)/(190/11)) + 1
	Ramp_UP_CAP_DELTA = Ramp_UP_Range_CODES_Needed - 4
	Ramp_DN_CAP_DELTA = Ramp_DN_Range_CODES_Needed - 7

	board.WriteI2C(desAlias0,0x40,0x3c)
	board.WriteI2C(desAlias0,0x41,0xf5)
	board.WriteI2C(desAlias0,0x42,(Efuse_TS_CODE<<4)+1) # Override TS_CODE Efuse Code
	if Ramp_UP_CAP_DELTA > 0:
	  TS_CODE_UP = Efuse_TS_CODE - Ramp_UP_CAP_DELTA
	  if TS_CODE_UP < 0:
		  TS_CODE_UP = 0
	  board.WriteI2C(desAlias0,0x41,0xf5)
	  rb = board.ReadI2C(desAlias0,0x42,1)
	  rb &= 0x8F
	  rb |= (TS_CODE_UP << 4)
	  board.WriteI2C(desAlias0,0x42,rb)
	  rb = board.ReadI2C(desAlias0,0x42,1)
	  rb &= 0xFE
	  rb |= 0x01
	  board.WriteI2C(desAlias0,0x42,rb)
	  board.WriteI2C(desAlias0,0x01,0x01)
	  time.sleep(0.04)
	if Ramp_DN_CAP_DELTA > 0:
	  TS_CODE_DN = Efuse_TS_CODE + Ramp_DN_CAP_DELTA
	  if TS_CODE_DN >= 7:
		  TS_CODE_DN = 7
	  board.WriteI2C(desAlias0,0x41,0xf5)
	  rb = board.ReadI2C(desAlias0,0x42,1)
	  rb &= 0x8F
	  rb |= (TS_CODE_DN << 4)
	  board.WriteI2C(desAlias0,0x42,rb)
	  rb = board.ReadI2C(desAlias0,0x42,1)
	  rb &= 0xFE
	  rb |= 0x01
	  board.WriteI2C(desAlias0,0x42,rb)
	  board.WriteI2C(desAlias0,0x01,0x01)
	  time.sleep(0.04)
	## *********************************************
	## Clear CRC errors from initial link process
	## *********************************************

	Reg_value = board.ReadI2C(serAddr,0x02,1)
	Reg_value = Reg_value | 0x20
	board.WriteI2C(serAddr,0x02,Reg_value) #CRC Error Reset

	Reg_value = board.ReadI2C(serAddr,0x02,1)
	Reg_value = Reg_value & 0xdf
	board.WriteI2C(serAddr,0x02,Reg_value) #CRC Error Reset Clear
	board.WriteI2C(serAddr,0x2d,0x01)

	## *********************************************
	## Hold Des DTG in reset
	## *********************************************
	board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
	board.WriteI2C(desAlias0,0x41,0x32)
	board.WriteI2C(desAlias0,0x42,0x06) #Hold Port 0 DTG in reset
	board.WriteI2C(desAlias0,0x41,0x62)
	board.WriteI2C(desAlias0,0x42,0x06) #Hold Port 1 DTG in reset
	## *********************************************
	## Disable Stream Mapping
	## *********************************************
	board.WriteI2C(desAlias0,0x0e,0x03) #Select both Output Ports
	board.WriteI2C(desAlias0,0xd0,0x00) #Disable FPD4 video forward to Output Port
	board.WriteI2C(desAlias0,0xd7,0x00) #Disable FPD3 video forward to Output Port
	## *********************************************
	## Force DP Rate
	## *********************************************
	board.WriteI2C(desAlias0,0x40,0x2c) #Select DP Page
	board.WriteI2C(desAlias0,0x41,0x81)
	board.WriteI2C(desAlias0,0x42,0x60) #Set DP Rate to 2.7Gbps
	board.WriteI2C(desAlias0,0x41,0x82)
	board.WriteI2C(desAlias0,0x42,0x02) #Enable force DP rate
	board.WriteI2C(desAlias0,0x41,0x91)
	board.WriteI2C(desAlias0,0x42,0x0c) #Force 4 lanes
	board.WriteI2C(desAlias0,0x40,0x30) #Disable DP SSCG
	board.WriteI2C(desAlias0,0x41,0x0f)
	board.WriteI2C(desAlias0,0x42,0x01)
	board.WriteI2C(desAlias0,0x01,0x40)
	## *********************************************
	## Setup DP ports
	## *********************************************
	board.WriteI2C(desAlias0,0x0e,0x12) #Select Port 1 registers
	board.WriteI2C(desAlias0,0x46,0x00) #Disable DP Port 1
	board.WriteI2C(desAlias0,0xe,0x01) #Select Port 0 registers
	board.WriteI2C(desAlias0,0x01,0x40) #DP-TX-PLL RESET Applied
	## *********************************************
	## Map video to display output
	## *********************************************
	board.WriteI2C(desAlias0,0x0e,0x03) #Select both Output Ports
	board.WriteI2C(desAlias0,0xd0,0x0c) #Enable FPD_RX video forward to Output Port
	board.WriteI2C(desAlias0,0xd1,0x0f) #Every stream forwarded on DC
	board.WriteI2C(desAlias0,0xd6,0x08) #Send Stream 0 to Output Port 0 and Send Stream 1 to Output Port 1
	board.WriteI2C(desAlias0,0xd7,0x00) #FPD3 mapping disabled
	board.WriteI2C(desAlias0,0x0e,0x01) #Select Port 0 
	## *********************************************
	## Program quad pixel clock for DP port 0
	## *********************************************
	board.WriteI2C(desAlias0,0x0e,0x01) #Select Port0 registers
	board.WriteI2C(desAlias0,0xb1,0x01) #Enable clock divider
	dp_mvalue = int(desPCLK * 1000)
	dp_nvalue = int(2.7 * 400000) 
	print "DES QPCLK m_value: ", dp_mvalue
	print "DES QPCLK n_value: ", dp_nvalue
	board.WriteI2C(desAlias0,0xb2,lower_byte(dp_mvalue)) #Program M value lower byte (04)
	board.WriteI2C(desAlias0,0xb3,upper_byte(dp_mvalue)) #Program M value middle byte (78)
	board.WriteI2C(desAlias0,0xb4,upper_byte_24(dp_mvalue)) #Program M value upper byte (01)
	board.WriteI2C(desAlias0,0xb5,lower_byte(dp_nvalue)) #Program N value lower byte (c0)
	board.WriteI2C(desAlias0,0xb6,upper_byte(dp_nvalue)) #Program N value middle byte (7a)
	board.WriteI2C(desAlias0,0xb7,upper_byte_24(dp_nvalue)) #Program N value upper byte (10)
	board.WriteI2C(desAlias0,0x0e,0x01) #Select Port 0 registers


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


	## *********************************************
	## Program DPTX for DP port 0
	## *********************************************
	board.WriteI2C(desAlias0,0x48,0x01) #Enable APB interface
	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xa4) #Set bit per color
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x40)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xb8) #Set pixel width
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x04)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	nvid=32768
	mvid = int(desPCLK * 10.0 / (2.7*1000)*nvid)
	print "DES DP Mvid: ", mvid
	board.WriteI2C(desAlias0,0x49,0xac) #Set DP Mvid
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,int((mvid & 0x000000FF)>>0)) 
	board.WriteI2C(desAlias0,0x4c,int((mvid & 0x0000FF00)>>8)) 
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)


	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xb4) #Set DP Nvid
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x00)
	board.WriteI2C(desAlias0,0x4c,0x80)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xc8) #Set TU Mode
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x00)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xb0) #Set TU Size
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x40)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x3e)
	board.WriteI2C(desAlias0,0x4e,0x0f)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xc8) #Set FIFO Size
	board.WriteI2C(desAlias0,0x4a,0x00)
	board.WriteI2C(desAlias0,0x4b,0x10)
	board.WriteI2C(desAlias0,0x4c,0x10)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xbc) #Set data count
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x74)
	board.WriteI2C(desAlias0,0x4c,0x0c)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xc0) #Disable STREAM INTERLACED
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x00)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0xc4) #Set SYNC polarity
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0x0c)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)
	## *********************************************
	## Release Des DTG reset
	## *********************************************
	board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
	board.WriteI2C(desAlias0,0x41,0x32)
	board.WriteI2C(desAlias0,0x42,0x04) #Release Port 0 DTG
	board.WriteI2C(desAlias0,0x41,0x62)
	board.WriteI2C(desAlias0,0x42,0x04) #Release Port 1 DTG
	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0x80) #Set Htotal
	board.WriteI2C(desAlias0,0x4a,0x01)
	board.WriteI2C(desAlias0,0x4b,0xb0)
	board.WriteI2C(desAlias0,0x4c,0x0d)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)
	## *********************************************
	## Enable DP 0 output
	## *********************************************
	board.WriteI2C(desAlias0,0x48,0x01)
	board.WriteI2C(desAlias0,0x49,0x84) #Enable DP output
	board.WriteI2C(desAlias0,0x4a,0x00)
	board.WriteI2C(desAlias0,0x4b,0x01)
	board.WriteI2C(desAlias0,0x4c,0x00)
	board.WriteI2C(desAlias0,0x4d,0x00)
	board.WriteI2C(desAlias0,0x4e,0x00)

		
sweepPCLK(283,283.4) # sweepPCLK(DES,SER)
board.WriteI2C(desAlias0,0x01,0x01)
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