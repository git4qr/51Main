################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/test_main_process.cpp 

OBJS += \
./src/test_main_process.o 

CPP_DEPS += \
./src/test_main_process.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -I../src/port -I/usr/include/GL -I/usr/local/cuda-8.0/include -O3 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -I../src/port -I/usr/include/GL -I/usr/local/cuda-8.0/include -O3 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


