

import time 

desAlias0 = 0x58
serAddr   = 0x18

board.WriteI2C(serAddr,0x2D,0x01)
board.WriteI2C(serAddr,0x70,0x58)
board.WriteI2C(serAddr,0x78,0x58)
board.WriteI2C(serAddr,0x88,0x0)
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
board.WriteI2C(serAddr,0x7,0x98)
board.WriteI2C(desAlias0,0x1,0x1) #Soft reset Des
time.sleep(1)
board.WriteI2C(serAddr,0x40,0x2e) #Select Ser link layer
board.WriteI2C(serAddr,0x41,0x0)
board.WriteI2C(serAddr,0x42,0x3) #refresh time slot
board.WriteI2C(serAddr,0x2d,0x1) #Select write to port0 reg
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
time.sleep(1) 
board.WriteI2C(serAddr,0x40,0x32)
board.WriteI2C(serAddr,0x41,0x1)
board.WriteI2C(serAddr,0x42,0xa8) #Set VP_SRC_SELECT to Stream 0 for SST Mode
board.WriteI2C(serAddr,0x41,0x2)
board.WriteI2C(serAddr,0x42,0xd0) #VID H Active
board.WriteI2C(serAddr,0x42,0x7) #VID H Active
board.WriteI2C(serAddr,0x41,0x10)
board.WriteI2C(serAddr,0x42,0xd0) #Horizontal Active
board.WriteI2C(serAddr,0x42,0x7) #Horizontal Active
board.WriteI2C(serAddr,0x42,0x20) #Horizontal Back Porch
board.WriteI2C(serAddr,0x42,0x0) #Horizontal Back Porch
board.WriteI2C(serAddr,0x42,0x10) #Horizontal Sync
board.WriteI2C(serAddr,0x42,0x0) #Horizontal Sync
board.WriteI2C(serAddr,0x42,0x24) #Horizontal Total
board.WriteI2C(serAddr,0x42,0x8) #Horizontal Total
board.WriteI2C(serAddr,0x42,0xce) #Vertical Active
board.WriteI2C(serAddr,0x42,0x3) #Vertical Active
board.WriteI2C(serAddr,0x42,0x0a) #Vertical Back Porch
board.WriteI2C(serAddr,0x42,0x0) #Vertical Back Porch
board.WriteI2C(serAddr,0x42,0x2) #Vertical Sync
board.WriteI2C(serAddr,0x42,0x0) #Vertical Sync
board.WriteI2C(serAddr,0x42,0x4) #Vertical Front Porch
board.WriteI2C(serAddr,0x42,0x0) #Vertical Front Porch
board.WriteI2C(serAddr,0x41,0x27)
board.WriteI2C(serAddr,0x42,0x0) #HSYNC Polarity = +, VSYNC Polarity = +
board.WriteI2C(serAddr,0x41,0x23) #M/N Register
board.WriteI2C(serAddr,0x42,0x79) #M value
board.WriteI2C(serAddr,0x42,0x17) #M value
board.WriteI2C(serAddr,0x42,0xf) #N value
board.WriteI2C(serAddr,0x43,0x0) #Set number of VPs used = 1
board.WriteI2C(serAddr,0x44,0x1) #Enable video processors
board.WriteI2C(serAddr,0x49,0x54) #Video Input Reset (should be executed after DP video is available from the source)
board.WriteI2C(serAddr,0x4a,0x0)
board.WriteI2C(serAddr,0x4b,0x1)
board.WriteI2C(serAddr,0x4c,0x0)
board.WriteI2C(serAddr,0x4d,0x0)
board.WriteI2C(serAddr,0x4e,0x0)
board.WriteI2C(serAddr,0x40,0x30)
board.WriteI2C(serAddr,0x41,0x29)
board.WriteI2C(serAddr,0x42,0x09) #Set PATGEN Color Depth to 24bpp for VP0
board.WriteI2C(serAddr,0x41,0x28)
board.WriteI2C(serAddr,0x42,0x95) #Enable PATGEN on VP0 - Comment out this line to disable PATGEN and enable end to end video
board.WriteI2C(serAddr,0x1,0x30) #Reset PLLs
board.WriteI2C(serAddr,0x40,0x2e) #Link layer Reg page
board.WriteI2C(serAddr,0x41,0x1) #Link layer stream enable
board.WriteI2C(serAddr,0x42,0x1) #Link layer stream enable
board.WriteI2C(serAddr,0x41,0x6) #Link layer time slot 0
board.WriteI2C(serAddr,0x42,0x3c) #Link layer time slot
board.WriteI2C(serAddr,0x41,0x20) #Set Link layer vp bpp
board.WriteI2C(serAddr,0x42,0x55) #Set Link layer vp bpp according to VP Bit per pixel
board.WriteI2C(serAddr,0x41,0x0) #Link layer enable
board.WriteI2C(serAddr,0x42,0x3) #Link layer enable
board.WriteI2C(desAlias0,0x40,0x6c)
board.WriteI2C(desAlias0,0x41,0xd)
board.WriteI2C(desAlias0,0x42,0x0)
board.WriteI2C(desAlias0,0x41,0x13)
board.WriteI2C(serAddr,0x2d,0x1)
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x32)
board.WriteI2C(desAlias0,0x42,0x2) #Hold Port 0 DTG in reset
board.WriteI2C(desAlias0,0x41,0x62)
board.WriteI2C(desAlias0,0x42,0x2) #Hold Port 1 DTG in reset
board.WriteI2C(desAlias0,0xe,0x3) #Select both Output Ports
board.WriteI2C(desAlias0,0xd0,0x0) #Disable FPD4 video forward to Output Port
board.WriteI2C(desAlias0,0xd7,0x0) #Disable FPD3 video forward to Output Port
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x20)
board.WriteI2C(desAlias0,0x42,0xa3) #DTG detect HS active high and VS active high
board.WriteI2C(desAlias0,0x41,0x29) #Set Hstart
board.WriteI2C(desAlias0,0x42,0x80) #Hstart upper byte
board.WriteI2C(desAlias0,0x41,0x2a)
board.WriteI2C(desAlias0,0x42,0x30) #Hstart lower byte
board.WriteI2C(desAlias0,0x41,0x2f) #Set HSW
board.WriteI2C(desAlias0,0x42,0x40) #HSW upper byte
board.WriteI2C(desAlias0,0x41,0x30)
board.WriteI2C(desAlias0,0x42,0x10) #HSW lower byte
board.WriteI2C(desAlias0,0xe,0x3) #Select both Output Ports
board.WriteI2C(desAlias0,0xd0,0xc) #Enable FPD_RX video forward to Output Port
board.WriteI2C(desAlias0,0xd1,0xf) #Every stream forwarded on DC
board.WriteI2C(desAlias0,0xd6,0x0) #Send Stream 0 to Output Port 0 and Send Stream 0 to Output Port 1
board.WriteI2C(desAlias0,0xd7,0x0) #FPD3 mapping disabled
board.WriteI2C(desAlias0,0xe,0x1) #Select Port 0 
board.WriteI2C(desAlias0,0x40,0x2c) #Configure OLDI/RGB Port Settings
board.WriteI2C(desAlias0,0x41,0x0)
board.WriteI2C(desAlias0,0x42,0x2F)
board.WriteI2C(desAlias0,0x41,0x1)
board.WriteI2C(desAlias0,0x42,0x2F)
board.WriteI2C(desAlias0,0x40,0x2e) #Configure OLDI/RGB PLL
board.WriteI2C(desAlias0,0x41,0x8)
board.WriteI2C(desAlias0,0x42,0x17) #PLL_NUM23_16
board.WriteI2C(desAlias0,0x42,0x53) #PLL_NUM15_8
board.WriteI2C(desAlias0,0x42,0x0D) #PLL_NUM7_0
board.WriteI2C(desAlias0,0x42,0xff) #PLL_DEN23_16
board.WriteI2C(desAlias0,0x42,0xf7) #PLL_DEN15_8
board.WriteI2C(desAlias0,0x42,0xbc) #PLL_DEN7_0
board.WriteI2C(desAlias0,0x41,0x18)
board.WriteI2C(desAlias0,0x42,0x20) #PLL_NDIV
board.WriteI2C(desAlias0,0x41,0x2d)
board.WriteI2C(desAlias0,0x42,0x11) #TX_SEL_CLKDIV
board.WriteI2C(desAlias0,0x40,0x50) #Configure Pixel Size
board.WriteI2C(desAlias0,0x41,0x20)
board.WriteI2C(desAlias0,0x42,0x53)
board.WriteI2C(desAlias0,0x40,0x50) #Select DTG Page
board.WriteI2C(desAlias0,0x41,0x32)
board.WriteI2C(desAlias0,0x42,0x0) #Release Des DTG reset
board.WriteI2C(desAlias0,0x41,0x62)
board.WriteI2C(desAlias0,0x42,0x0) #Release Des DTG reset
board.WriteI2C(desAlias0,0x1,0x40) #OLDI Reset
board.WriteI2C(desAlias0,0x40,0x2c) #Enable OLDI/RGB
board.WriteI2C(desAlias0,0x41,0x2)
board.WriteI2C(desAlias0,0x42,0x14)
board.WriteI2C(desAlias0,0x41,0x20) # P0 TX_EN (from strap?)
board.WriteI2C(desAlias0,0x42,0x80)
board.WriteI2C(desAlias0,0x41,0x22) # P1 TX_EN (from strap?)
board.WriteI2C(desAlias0,0x42,0x80)
board.WriteI2C(serAddr, 0x40, 0x30)
board.WriteI2C(serAddr, 0x41, 0x28)
board.WriteI2C(serAddr, 0x42, 0x95)
board.WriteI2C(serAddr, 0x41, 0x29)
board.WriteI2C(serAddr, 0x42, 0xc8)

board.WriteI2C(serAddr, 0x40, 0x30)
board.WriteI2C(serAddr, 0x41, 0x68)
board.WriteI2C(serAddr, 0x42, 0x05)
board.WriteI2C(serAddr, 0x41, 0x69)
board.WriteI2C(serAddr, 0x42, 0x08)

#tp
board.WriteI2C(serAddr,0x07,0x98)
board.WriteI2C(serAddr,0x70,0x24)
board.WriteI2C(serAddr,0x78,0x24) #Set PATGEN Color Depth to 24bpp for VP0
board.WriteI2C(serAddr,0x71,0x96)
board.WriteI2C(serAddr,0x79,0x96) #Enab

board.WriteI2C(serAddr,0x1D,0x89)
