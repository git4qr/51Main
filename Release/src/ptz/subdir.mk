################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ptz/PTZ_control.cpp \
../src/ptz/PTZ_speedTransfer.cpp \
../src/ptz/pelcoProtocol.cpp 

OBJS += \
./src/ptz/PTZ_control.o \
./src/ptz/PTZ_speedTransfer.o \
./src/ptz/pelcoProtocol.o 

CPP_DEPS += \
./src/ptz/PTZ_control.d \
./src/ptz/PTZ_speedTransfer.d \
./src/ptz/pelcoProtocol.d 


# Each subdirectory must supply rules for building sources it contributes
src/ptz/%.o: ../src/ptz/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/plantCtrl -I../src/port -I../src/ptz -I../src/OSA_CAP/inc -O3 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/ptz" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/plantCtrl -I../src/port -I../src/ptz -I../src/OSA_CAP/inc -O3 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


