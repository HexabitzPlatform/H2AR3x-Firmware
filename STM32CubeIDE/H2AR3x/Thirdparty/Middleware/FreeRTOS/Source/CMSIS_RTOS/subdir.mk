################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/User/Desktop/hexa_projects/Modules_firmware/H2AR3-Firmware-master/Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c 

OBJS += \
./Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.o 

C_DEPS += \
./Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.d 


# Each subdirectory must supply rules for building sources it contributes
Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.o: C:/Users/User/Desktop/hexa_projects/Modules_firmware/H2AR3-Firmware-master/Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=c99 -g3 -DUSE_I2C_2V8 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F091xB -DSTM32F091xC '-D_module=1' -DSTM32 -DSTM32F0 -DSTM32F091CBUx -DH2AR3 -c -I../../Thirdparty/CMSIS/Include -I../../Thirdparty/CMSIS/Device/ST/STM32F0xx/Include -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Thirdparty/STM32F0xx_HAL_Driver/Inc -I../../Thirdparty/Middleware/FreeRTOS/Source/include -I../../Thirdparty/Middleware/FreeRTOS/Source/portable/GCC/ARM_CM0 -I../../Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS -I../../BOS -I../../User -I../../Thirdparty/VL53L0X_1.0.2/Api/core/inc -I../../Thirdparty/VL53L0X_1.0.2/Api/platform/inc -I../Inc -I../../H2AR3 -O1 -ffunction-sections -fdata-sections -fstack-usage -MMD -MP -MF"Thirdparty/Middleware/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

