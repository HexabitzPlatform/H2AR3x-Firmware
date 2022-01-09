################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
D:/Hexabitz\ release/H2AR3x/H2AR3/startup_stm32f091xc.s 

C_SRCS += \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_adc.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_dma.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_gpio.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_it.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_rtc.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_timers.c \
D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_uart.c 

OBJS += \
./H2AR3/H2AR3.o \
./H2AR3/H2AR3_adc.o \
./H2AR3/H2AR3_dma.o \
./H2AR3/H2AR3_gpio.o \
./H2AR3/H2AR3_it.o \
./H2AR3/H2AR3_rtc.o \
./H2AR3/H2AR3_timers.o \
./H2AR3/H2AR3_uart.o \
./H2AR3/startup_stm32f091xc.o 

S_DEPS += \
./H2AR3/startup_stm32f091xc.d 

C_DEPS += \
./H2AR3/H2AR3.d \
./H2AR3/H2AR3_adc.d \
./H2AR3/H2AR3_dma.d \
./H2AR3/H2AR3_gpio.d \
./H2AR3/H2AR3_it.d \
./H2AR3/H2AR3_rtc.d \
./H2AR3/H2AR3_timers.d \
./H2AR3/H2AR3_uart.d 


# Each subdirectory must supply rules for building sources it contributes
H2AR3/H2AR3.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_adc.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_adc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_adc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_dma.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_dma.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_dma.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_gpio.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_gpio.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_gpio.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_it.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_it.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_rtc.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_rtc.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_rtc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_timers.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_timers.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_timers.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/H2AR3_uart.o: D:/Hexabitz\ release/H2AR3x/H2AR3/H2AR3_uart.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"H2AR3/H2AR3_uart.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
H2AR3/startup_stm32f091xc.o: D:/Hexabitz\ release/H2AR3x/H2AR3/startup_stm32f091xc.s
	arm-none-eabi-gcc -mcpu=cortex-m0 -g -c -x assembler-with-cpp -MMD -MP -MF"H2AR3/startup_stm32f091xc.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

