################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/Events.c \
../Sources/FTM.c \
../Sources/PIT.c \
../Sources/RTC.c \
../Sources/main.c 

OBJS += \
./Sources/Events.o \
./Sources/FTM.o \
./Sources/PIT.o \
./Sources/RTC.o \
./Sources/main.o 

C_DEPS += \
./Sources/Events.d \
./Sources/FTM.d \
./Sources/PIT.d \
./Sources/RTC.d \
./Sources/main.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -I"C:/Users/Harindi Perera/git/es18aut35/Lab3/Static_Code/IO_Map" -I"C:/Users/Harindi Perera/git/es18aut35/Lab3/Sources" -I"C:/Users/Harindi Perera/git/es18aut35/Lab3/Generated_Code" -I"C:/Users/Harindi Perera/git/es18aut35/Lab3/Static_Code/PDD" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


