################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.c \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.c \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.c \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.c \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.c \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.c \
C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.c 

OBJS += \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.o \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.o \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.o \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.o \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.o \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.o \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.o 

C_DEPS += \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.d \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.d \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.d \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.d \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.d \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.d \
./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.o: C:/Users/ASUS/Desktop/H2AR3/Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.c Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC -D_module=2 -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -I../../Thirdparty/CMSIS/STM32F0xx/Include -I../../Thirdparty/CMSIS/STM32F0xx/Device/ST/STM32F0xx/Include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/include -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/CMSIS_RTOS -Og -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Thirdparty-2f-Middleware-2f-STM32F0xx-2f-FreeRTOS-2f-Source

clean-Thirdparty-2f-Middleware-2f-STM32F0xx-2f-FreeRTOS-2f-Source:
	-$(RM) ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/FreeRTOS_CLI.o ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/croutine.o ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/event_groups.o ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/list.o ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/queue.o ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/tasks.o ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.d ./Thirdparty/Middleware/STM32F0xx/FreeRTOS/Source/timers.o

.PHONY: clean-Thirdparty-2f-Middleware-2f-STM32F0xx-2f-FreeRTOS-2f-Source

