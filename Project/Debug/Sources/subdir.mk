################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/FIFO.c \
../Sources/FTM.c \
../Sources/Flash.c \
../Sources/LEDs.c \
../Sources/PIT.c \
../Sources/SCP.c \
../Sources/UART.c \
../Sources/UsefulFunctions.c \
../Sources/analogmeasure.c \
../Sources/main.c \
../Sources/packet.c \
../Sources/signals.c \
../Sources/voltageRegulator.c 

OBJS += \
./Sources/FIFO.o \
./Sources/FTM.o \
./Sources/Flash.o \
./Sources/LEDs.o \
./Sources/PIT.o \
./Sources/SCP.o \
./Sources/UART.o \
./Sources/UsefulFunctions.o \
./Sources/analogmeasure.o \
./Sources/main.o \
./Sources/packet.o \
./Sources/signals.o \
./Sources/voltageRegulator.o 

C_DEPS += \
./Sources/FIFO.d \
./Sources/FTM.d \
./Sources/Flash.d \
./Sources/LEDs.d \
./Sources/PIT.d \
./Sources/SCP.d \
./Sources/UART.d \
./Sources/UsefulFunctions.d \
./Sources/analogmeasure.d \
./Sources/main.d \
./Sources/packet.d \
./Sources/signals.d \
./Sources/voltageRegulator.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\12443166\Documents\git\es18aut35\Project\Library" -I"C:/Users/12443166/Documents/git/es18aut35/Project/Static_Code/IO_Map" -I"C:/Users/12443166/Documents/git/es18aut35/Project/Sources" -I"C:/Users/12443166/Documents/git/es18aut35/Project/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


