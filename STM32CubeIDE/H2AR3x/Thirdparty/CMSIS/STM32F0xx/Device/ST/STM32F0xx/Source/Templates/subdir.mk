################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Hexabitz/for\ Release/Modules\ firmware/H2AR3x/Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c 

OBJS += \
./Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.o 

C_DEPS += \
./Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.o: D:/Hexabitz/for\ Release/Modules\ firmware/H2AR3x/Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=1 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Thirdparty-2f-CMSIS-2f-STM32F0xx-2f-Device-2f-ST-2f-STM32F0xx-2f-Source-2f-Templates

clean-Thirdparty-2f-CMSIS-2f-STM32F0xx-2f-Device-2f-ST-2f-STM32F0xx-2f-Source-2f-Templates:
	-$(RM) ./Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.d ./Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.o

.PHONY: clean-Thirdparty-2f-CMSIS-2f-STM32F0xx-2f-Device-2f-ST-2f-STM32F0xx-2f-Source-2f-Templates

