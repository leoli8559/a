################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/web/subdir.mk
-include src/phy/subdir.mk
-include src/ota/subdir.mk
-include src/api/subdir.mk
-include src/subdir.mk
-include generate/src/subdir.mk
-include RTD/src/subdir.mk
-include Project_Settings/Startup_Code/subdir.mk
-include FreeRTOS/Source/portable/MemMang/subdir.mk
-include FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/subdir.mk
-include FreeRTOS/Source/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Switch_s32k_5152.bin \

SECONDARY_SIZE += \
Switch_s32k_5152.siz \


# All Target
all: Switch_s32k_5152.elf secondary-outputs

# Tool invocations
Switch_s32k_5152.elf: $(OBJS) C:/Users/sigent/Desktop/Switch_s32k_5152/Project_Settings/Linker_Files/linker_flash_s32k314.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Standard S32DS C Linker'
	arm-none-eabi-gcc -o "Switch_s32k_5152.elf" "@Switch_s32k_5152.args"  $(USER_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

Switch_s32k_5152.bin: Switch_s32k_5152.elf
	@echo 'Invoking: Standard S32DS Create Flash Image'
	arm-none-eabi-objcopy -O binary Switch_s32k_5152.elf  "Switch_s32k_5152.bin"
	@echo 'Finished building: $@'
	@echo ' '

Switch_s32k_5152.siz: Switch_s32k_5152.elf
	@echo 'Invoking: Standard S32DS Print Size'
	arm-none-eabi-size --format=berkeley Switch_s32k_5152.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ./*
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
