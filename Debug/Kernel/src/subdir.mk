################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Kernel/src/Kernel.c 

OBJS += \
./Kernel/src/Kernel.o 

C_DEPS += \
./Kernel/src/Kernel.d 


# Each subdirectory must supply rules for building sources it contributes
Kernel/src/%.o: ../Kernel/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=gnu11 -I"/home/utnso/git/tp-2017-1c-System-Buzz/Common" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


