################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CircularBuffer.cpp \
../src/LoRa.cpp \
../src/MainMCU.cpp \
../src/UART.cpp 

OBJS += \
./src/CircularBuffer.o \
./src/LoRa.o \
./src/MainMCU.o \
./src/UART.o 

CPP_DEPS += \
./src/CircularBuffer.d \
./src/LoRa.d \
./src/MainMCU.d \
./src/UART.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o src/%.su src/%.cyclo: ../src/%.cpp src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I"C:/Users/Daniel/repos/E220_900T22D_STM32/src" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-src

clean-src:
	-$(RM) ./src/CircularBuffer.cyclo ./src/CircularBuffer.d ./src/CircularBuffer.o ./src/CircularBuffer.su ./src/LoRa.cyclo ./src/LoRa.d ./src/LoRa.o ./src/LoRa.su ./src/MainMCU.cyclo ./src/MainMCU.d ./src/MainMCU.o ./src/MainMCU.su ./src/UART.cyclo ./src/UART.d ./src/UART.o ./src/UART.su

.PHONY: clean-src

