################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IPC/ipcProc.cpp 

OBJS += \
./src/IPC/ipcProc.o 

CPP_DEPS += \
./src/IPC/ipcProc.d 


# Each subdirectory must supply rules for building sources it contributes
src/IPC/%.o: ../src/IPC/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -I../src/port -I/usr/include/GL -I/usr/local/cuda-8.0/include -G -g -O0 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/IPC" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -I../src/port -I/usr/include/GL -I/usr/local/cuda-8.0/include -G -g -O0 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


