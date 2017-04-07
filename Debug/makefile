################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Memoria/src/subdir.mk
-include Memoria/Debug/src/subdir.mk
-include Kernel/src/subdir.mk
-include Kernel/Debug/src/subdir.mk
-include File_System/src/subdir.mk
-include File_System/Debug/src/subdir.mk
-include Consola/src/subdir.mk
-include Consola/Debug/src/subdir.mk
-include Common/subdir.mk
-include CPU/src/subdir.mk
-include CPU/Debug/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libtp-2017-1c-System-Buzz.so

# Tool invocations
libtp-2017-1c-System-Buzz.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc -shared -o "libtp-2017-1c-System-Buzz.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(LIBRARIES) libtp-2017-1c-System-Buzz.so
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
