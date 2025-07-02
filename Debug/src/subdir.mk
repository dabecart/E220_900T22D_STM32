################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CircularBuffer.c \
../src/LoRa.c \
../src/MainMCU.c \
../src/UART.c 

C_DEPS += \
./src/CircularBuffer.d \
./src/LoRa.d \
./src/MainMCU.d \
./src/UART.d 

OBJS += \
./src/CircularBuffer.o \
./src/LoRa.o \
./src/MainMCU.o \
./src/UART.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o src/%.su src/%.cyclo: ../src/%.c src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I"C:/Users/Daniel/repos/E220_900T22D_STM32/src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src

clean-src:
	-$(RM) ./src/CircularBuffer.cyclo ./src/CircularBuffer.d ./src/CircularBuffer.o ./src/CircularBuffer.su ./src/LoRa.cyclo ./src/LoRa.d ./src/LoRa.o ./src/LoRa.su ./src/MainMCU.cyclo ./src/MainMCU.d ./src/MainMCU.o ./src/MainMCU.su ./src/UART.cyclo ./src/UART.d ./src/UART.o ./src/UART.su

.PHONY: clean-src

