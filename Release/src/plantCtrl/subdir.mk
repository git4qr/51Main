################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/plantCtrl/Kalman.c \
../src/plantCtrl/deviceUser.c \
../src/plantCtrl/platformControl.c \
../src/plantCtrl/platformDebug.c \
../src/plantCtrl/platformFilter.c \
../src/plantCtrl/sensorComp.c 

OBJS += \
./src/plantCtrl/Kalman.o \
./src/plantCtrl/deviceUser.o \
./src/plantCtrl/platformControl.o \
./src/plantCtrl/platformDebug.o \
./src/plantCtrl/platformFilter.o \
./src/plantCtrl/sensorComp.o 

C_DEPS += \
./src/plantCtrl/Kalman.d \
./src/plantCtrl/deviceUser.d \
./src/plantCtrl/platformControl.d \
./src/plantCtrl/platformDebug.d \
./src/plantCtrl/platformFilter.d \
./src/plantCtrl/sensorComp.d 


# Each subdirectory must supply rules for building sources it contributes
src/plantCtrl/%.o: ../src/plantCtrl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/plantCtrl -I../src/port -I../src/ptz -I../src/OSA_CAP/inc -O3 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/plantCtrl" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/plantCtrl -I../src/port -I../src/ptz -I../src/OSA_CAP/inc -O3 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


