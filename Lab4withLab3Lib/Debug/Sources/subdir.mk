################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/I2C.c \
../Sources/SCP.c \
../Sources/accel.c \
../Sources/main.c \
../Sources/median.c 

OBJS += \
./Sources/I2C.o \
./Sources/SCP.o \
./Sources/accel.o \
./Sources/main.o \
./Sources/median.o 

C_DEPS += \
./Sources/I2C.d \
./Sources/SCP.d \
./Sources/accel.d \
./Sources/main.d \
./Sources/median.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:\Users\12443166\Documents\gitrepo\es18aut35\Lab4withLab3Lib\Library" -I"C:/Users/12443166/Documents/gitrepo/es18aut35/Lab4withLab3Lib/Static_Code/IO_Map" -I"C:/Users/12443166/Documents/gitrepo/es18aut35/Lab4withLab3Lib/Sources" -I"C:/Users/12443166/Documents/gitrepo/es18aut35/Lab4withLab3Lib/Generated_Code" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


