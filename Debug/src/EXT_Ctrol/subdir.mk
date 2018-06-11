################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/EXT_Ctrol/CConnect.cpp \
../src/EXT_Ctrol/NetProcess.cpp \
../src/EXT_Ctrol/UartProcess.cpp \
../src/EXT_Ctrol/UserProcess.cpp \
../src/EXT_Ctrol/joystickProcess.cpp 

OBJS += \
./src/EXT_Ctrol/CConnect.o \
./src/EXT_Ctrol/NetProcess.o \
./src/EXT_Ctrol/UartProcess.o \
./src/EXT_Ctrol/UserProcess.o \
./src/EXT_Ctrol/joystickProcess.o 

CPP_DEPS += \
./src/EXT_Ctrol/CConnect.d \
./src/EXT_Ctrol/NetProcess.d \
./src/EXT_Ctrol/UartProcess.d \
./src/EXT_Ctrol/UserProcess.d \
./src/EXT_Ctrol/joystickProcess.d 


# Each subdirectory must supply rules for building sources it contributes
src/EXT_Ctrol/%.o: ../src/EXT_Ctrol/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -I../src/port -I/usr/include/GL -I/usr/local/cuda-8.0/include -G -g -O0 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/EXT_Ctrol" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I../include -I../src/OSA_IPC/inc -I../src/OSA_CAP/inc -I../src/plantCtrl -I../src/ptz -I../src/port -I/usr/include/GL -I/usr/local/cuda-8.0/include -G -g -O0 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


