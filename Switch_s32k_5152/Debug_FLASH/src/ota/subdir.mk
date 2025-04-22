################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ota/CRC_hal.c \
../src/ota/OTA.c \
../src/ota/app_hdr.c 

OBJS += \
./src/ota/CRC_hal.o \
./src/ota/OTA.o \
./src/ota/app_hdr.o 

C_DEPS += \
./src/ota/CRC_hal.d \
./src/ota/OTA.d \
./src/ota/app_hdr.d 


# Each subdirectory must supply rules for building sources it contributes
src/ota/%.o: ../src/ota/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/ota/CRC_hal.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


