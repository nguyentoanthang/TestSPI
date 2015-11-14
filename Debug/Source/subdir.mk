################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Source/PN532_SPI.c \
../Source/main.c 

OBJS += \
./Source/PN532_SPI.o \
./Source/main.o 

C_DEPS += \
./Source/PN532_SPI.d \
./Source/main.d 


# Each subdirectory must supply rules for building sources it contributes
Source/PN532_SPI.o: /Volumes/Data/Developer/C/TestSPI/Source/PN532_SPI.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/Volumes/Data/Developer/C/TestSPI/Lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Source/main.o: /Volumes/Data/Developer/C/TestSPI/Source/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/Volumes/Data/Developer/C/TestSPI/Lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


