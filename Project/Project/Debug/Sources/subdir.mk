################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/FIFO.c \
../Sources/FTM.c \
../Sources/Flash.c \
../Sources/Frequencie.c \
../Sources/LEDs.c \
../Sources/Output.c \
../Sources/PIT.c \
../Sources/RTC.c \
../Sources/Regulation.c \
../Sources/SCP.c \
../Sources/UART.c \
../Sources/main.c \
../Sources/packet.c 

OBJS += \
./Sources/FIFO.o \
./Sources/FTM.o \
./Sources/Flash.o \
./Sources/Frequencie.o \
./Sources/LEDs.o \
./Sources/Output.o \
./Sources/PIT.o \
./Sources/RTC.o \
./Sources/Regulation.o \
./Sources/SCP.o \
./Sources/UART.o \
./Sources/main.o \
./Sources/packet.o 

C_DEPS += \
./Sources/FIFO.d \
./Sources/FTM.d \
./Sources/Flash.d \
./Sources/Frequencie.d \
./Sources/LEDs.d \
./Sources/Output.d \
./Sources/PIT.d \
./Sources/RTC.d \
./Sources/Regulation.d \
./Sources/SCP.d \
./Sources/UART.d \
./Sources/main.d \
./Sources/packet.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\13115605\Documents\workspace\es18aut35\Project\Project\Library" -I"C:/Users/13115605/Documents/workspace/es18aut35/Project/Project/Static_Code/IO_Map" -I"C:/Users/13115605/Documents/workspace/es18aut35/Project/Project/Sources" -I"C:/Users/13115605/Documents/workspace/es18aut35/Project/Project/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


