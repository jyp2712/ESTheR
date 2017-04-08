################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../File_System/src/File_System.c 

OBJS += \
./File_System/src/File_System.o 

C_DEPS += \
./File_System/src/File_System.d 


# Each subdirectory must supply rules for building sources it contributes
File_System/src/%.o: ../File_System/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -std=gnu11 -I"/home/utnso/git/tp-2017-1c-System-Buzz/Common" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


