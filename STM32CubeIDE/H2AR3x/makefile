################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Thirdparty/STM32F0xx_HAL_Driver/Src/subdir.mk
-include Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0/subdir.mk
-include Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
-include Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/subdir.mk
-include H2AR3/subdir.mk
-include BOS/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := H2AR3x
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
H2AR3x.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
H2AR3x.list \

OBJCOPY_HEX += \
H2AR3x.hex \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: H2AR3x.elf secondary-outputs

# Tool invocations
H2AR3x.elf: $(OBJS) $(USER_OBJS) C:\Users\ASUS\Desktop\H2AR3\STM32CubeIDE\STM32F091CBUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "H2AR3x.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\ASUS\Desktop\H2AR3\STM32CubeIDE\STM32F091CBUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="H2AR3x.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

H2AR3x.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "H2AR3x.list"
	@echo 'Finished building: $@'
	@echo ' '

H2AR3x.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "H2AR3x.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) H2AR3x.elf H2AR3x.hex H2AR3x.list default.size.stdout
	-@echo ' '

post-build:
	arm-none-eabi-objcopy -O ihex H2AR3x.elf H2AR3x.hex
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
