################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Kernel.c \
../src/configuration.c \
../src/console.c \
../src/memory_layer.c \
../src/server.c 

OBJS += \
./src/Kernel.o \
./src/configuration.o \
./src/console.o \
./src/memory_layer.o \
./src/server.o 

C_DEPS += \
./src/Kernel.d \
./src/configuration.d \
./src/console.d \
./src/memory_layer.d \
./src/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/git/tp-2017-1c-System-Buzz/Common" -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


