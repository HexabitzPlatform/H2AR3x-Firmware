################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Hexabitz/for\ Release/Modules\ firmware/H2AR3x/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.c 

OBJS += \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.o 

C_DEPS += \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.d 


# Each subdirectory must supply rules for building sources it contributes
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.o: D:/Hexabitz/for\ Release/Modules\ firmware/H2AR3x/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=1 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Thirdparty-2f-Middleware-2f-STM32F0xx-2f-FreeRTOS-2f-Source-2f-portable-2f-MemMang

clean-Thirdparty-2f-Middleware-2f-STM32F0xx-2f-FreeRTOS-2f-Source-2f-portable-2f-MemMang:
	-$(RM) ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/MemMang/heap_4.o

.PHONY: clean-Thirdparty-2f-Middleware-2f-STM32F0xx-2f-FreeRTOS-2f-Source-2f-portable-2f-MemMang

