################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/utnso/workspace/tp-2017-1c-System-Buzz/Common/globals.c \
/home/utnso/workspace/tp-2017-1c-System-Buzz/Common/log.c \
/home/utnso/workspace/tp-2017-1c-System-Buzz/Common/protocol.c \
/home/utnso/workspace/tp-2017-1c-System-Buzz/Common/serial.c \
/home/utnso/workspace/tp-2017-1c-System-Buzz/Common/socket.c \
/home/utnso/workspace/tp-2017-1c-System-Buzz/Common/utils.c 

OBJS += \
./Common/globals.o \
./Common/log.o \
./Common/protocol.o \
./Common/serial.o \
./Common/socket.o \
./Common/utils.o 

C_DEPS += \
./Common/globals.d \
./Common/log.d \
./Common/protocol.d \
./Common/serial.d \
./Common/socket.d \
./Common/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Common/globals.o: /home/utnso/workspace/tp-2017-1c-System-Buzz/Common/globals.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/workspace/tp-2017-1c-System-Buzz/Common" -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Common/log.o: /home/utnso/workspace/tp-2017-1c-System-Buzz/Common/log.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/workspace/tp-2017-1c-System-Buzz/Common" -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Common/protocol.o: /home/utnso/workspace/tp-2017-1c-System-Buzz/Common/protocol.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/workspace/tp-2017-1c-System-Buzz/Common" -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Common/serial.o: /home/utnso/workspace/tp-2017-1c-System-Buzz/Common/serial.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/workspace/tp-2017-1c-System-Buzz/Common" -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Common/socket.o: /home/utnso/workspace/tp-2017-1c-System-Buzz/Common/socket.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/workspace/tp-2017-1c-System-Buzz/Common" -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Common/utils.o: /home/utnso/workspace/tp-2017-1c-System-Buzz/Common/utils.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -std=gnu11 -I"/home/utnso/workspace/tp-2017-1c-System-Buzz/Common" -O0 -g3 -pedantic -Wall -Wextra -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


