# AVR cross-compiling toolchain
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

set(CMAKE_C_COMPILER C:/msys64/mingw64/bin/avr-gcc.exe)
set(CMAKE_OBJCOPY C:/msys64/mingw64/bin/avr-objcopy.exe)

set(MCU atmega32u4)
set(F_CPU 16000000UL)

set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -std=c11")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU}")

# Prevent CMake from linking default system libraries
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
