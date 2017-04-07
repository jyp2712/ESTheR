################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/cdata.c \
../Common/socket.c \
../Common/utils.c 

OBJS += \
./Common/cdata.o \
./Common/socket.o \
./Common/utils.o 

C_DEPS += \
./Common/cdata.d \
./Common/socket.d \
./Common/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Common/%.o: ../Common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


