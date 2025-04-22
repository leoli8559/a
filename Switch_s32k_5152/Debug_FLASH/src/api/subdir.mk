################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/api/Fir_msdBrgFdb.c \
../src/api/Fir_msdBrgFdbIntf.c \
../src/api/Fir_msdBrgVtu.c \
../src/api/Fir_msdBrgVtuIntf.c \
../src/api/Fir_msdHwAccess.c \
../src/api/Fir_msdPortCtrl.c \
../src/api/Fir_msdPortCtrlIntf.c \
../src/api/Fir_msdSysCtrl.c \
../src/api/Fir_msdSysCtrlIntf.c \
../src/api/Fir_msdTCAM.c \
../src/api/Fir_msdTCAMIntf.c \
../src/api/adlist.c \
../src/api/apiInit.c \
../src/api/deviceFilterModule.c \
../src/api/deviceInfoModule.c \
../src/api/deviceMacModule.c \
../src/api/devicePortSegmentationModule.c \
../src/api/deviceVlanModule.c \
../src/api/msdBrgFdb.c \
../src/api/msdBrgVtu.c \
../src/api/msdDebug.c \
../src/api/msdDevObj.c \
../src/api/msdHwAccess.c \
../src/api/msdPortCtrl.c \
../src/api/msdQosMap.c \
../src/api/msdSem.c \
../src/api/msdSysConfig.c \
../src/api/msdSysCtrl.c \
../src/api/msdTCAM.c \
../src/api/msdUtils.c \
../src/api/msdVersion.c \
../src/api/smiaccess.c \
../src/api/umsdUtil.c 

OBJS += \
./src/api/Fir_msdBrgFdb.o \
./src/api/Fir_msdBrgFdbIntf.o \
./src/api/Fir_msdBrgVtu.o \
./src/api/Fir_msdBrgVtuIntf.o \
./src/api/Fir_msdHwAccess.o \
./src/api/Fir_msdPortCtrl.o \
./src/api/Fir_msdPortCtrlIntf.o \
./src/api/Fir_msdSysCtrl.o \
./src/api/Fir_msdSysCtrlIntf.o \
./src/api/Fir_msdTCAM.o \
./src/api/Fir_msdTCAMIntf.o \
./src/api/adlist.o \
./src/api/apiInit.o \
./src/api/deviceFilterModule.o \
./src/api/deviceInfoModule.o \
./src/api/deviceMacModule.o \
./src/api/devicePortSegmentationModule.o \
./src/api/deviceVlanModule.o \
./src/api/msdBrgFdb.o \
./src/api/msdBrgVtu.o \
./src/api/msdDebug.o \
./src/api/msdDevObj.o \
./src/api/msdHwAccess.o \
./src/api/msdPortCtrl.o \
./src/api/msdQosMap.o \
./src/api/msdSem.o \
./src/api/msdSysConfig.o \
./src/api/msdSysCtrl.o \
./src/api/msdTCAM.o \
./src/api/msdUtils.o \
./src/api/msdVersion.o \
./src/api/smiaccess.o \
./src/api/umsdUtil.o 

C_DEPS += \
./src/api/Fir_msdBrgFdb.d \
./src/api/Fir_msdBrgFdbIntf.d \
./src/api/Fir_msdBrgVtu.d \
./src/api/Fir_msdBrgVtuIntf.d \
./src/api/Fir_msdHwAccess.d \
./src/api/Fir_msdPortCtrl.d \
./src/api/Fir_msdPortCtrlIntf.d \
./src/api/Fir_msdSysCtrl.d \
./src/api/Fir_msdSysCtrlIntf.d \
./src/api/Fir_msdTCAM.d \
./src/api/Fir_msdTCAMIntf.d \
./src/api/adlist.d \
./src/api/apiInit.d \
./src/api/deviceFilterModule.d \
./src/api/deviceInfoModule.d \
./src/api/deviceMacModule.d \
./src/api/devicePortSegmentationModule.d \
./src/api/deviceVlanModule.d \
./src/api/msdBrgFdb.d \
./src/api/msdBrgVtu.d \
./src/api/msdDebug.d \
./src/api/msdDevObj.d \
./src/api/msdHwAccess.d \
./src/api/msdPortCtrl.d \
./src/api/msdQosMap.d \
./src/api/msdSem.d \
./src/api/msdSysConfig.d \
./src/api/msdSysCtrl.d \
./src/api/msdTCAM.d \
./src/api/msdUtils.d \
./src/api/msdVersion.d \
./src/api/smiaccess.d \
./src/api/umsdUtil.d 


# Each subdirectory must supply rules for building sources it contributes
src/api/%.o: ../src/api/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/api/Fir_msdBrgFdb.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


