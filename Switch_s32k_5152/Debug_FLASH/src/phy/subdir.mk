################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/phy/marvell_88e1512.c 

OBJS += \
./src/phy/marvell_88e1512.o 

C_DEPS += \
./src/phy/marvell_88e1512.d 


# Each subdirectory must supply rules for building sources it contributes
src/phy/%.o: ../src/phy/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/phy/marvell_88e1512.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


