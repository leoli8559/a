################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/web/driver_S32K314.c \
../src/web/mongoose.c \
../src/web/mongoose_fs.c \
../src/web/mongoose_glue.c \
../src/web/mongoose_impl.c \
../src/web/server.c 

OBJS += \
./src/web/driver_S32K314.o \
./src/web/mongoose.o \
./src/web/mongoose_fs.o \
./src/web/mongoose_glue.o \
./src/web/mongoose_impl.o \
./src/web/server.o 

C_DEPS += \
./src/web/driver_S32K314.d \
./src/web/mongoose.d \
./src/web/mongoose_fs.d \
./src/web/mongoose_glue.d \
./src/web/mongoose_impl.d \
./src/web/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/web/%.o: ../src/web/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@src/web/driver_S32K314.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


