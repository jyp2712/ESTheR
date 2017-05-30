################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Consola.c \
../src/console.c \
../src/console_config.c \
../src/message_receiver.c \
../src/program_handler.c 

OBJS += \
./src/Consola.o \
./src/console.o \
./src/console_config.o \
./src/message_receiver.o \
./src/program_handler.o 

C_DEPS += \
./src/Consola.d \
./src/console.d \
./src/console_config.d \
./src/message_receiver.d \
./src/program_handler.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/git/tp-2017-1c-System-Buzz/Common" -O0 -g3 -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


