################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/MSGDRI/msgDriv.cpp 

OBJS += \
./src/MSGDRI/msgDriv.o 

CPP_DEPS += \
./src/MSGDRI/msgDriv.d 


# Each subdirectory must supply rules for building sources it contributes
src/MSGDRI/%.o: ../src/MSGDRI/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/port -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/MSGDRI" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/port -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


