cmake_minimum_required(VERSION 3.15)
project(skb_avr_atmega32u4 C)

# Tell CMake this is AVR (bare metal, no OS)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# Cross-compiler paths
set(CMAKE_C_COMPILER C:/msys64/mingw64/bin/avr-gcc.exe)
set(CMAKE_OBJCOPY C:/msys64/mingw64/bin/avr-objcopy.exe)

# MCU settings
set(MCU atmega32u4)
set(F_CPU 16000000UL)

# Compiler & linker flags
set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -std=c11")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU}")


# Source files
# file(GLOB SOURCES "src/main.c")
# set(SOURCES  "src/main.c")

set(MASTER_SOURCES "src/master.c")
set(SLAVE_SOURCES "src/slave.c")

# Build ELF (raw, no .exe)
# add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Generate HEX file
# add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
# 	COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
#	COMMENT "Generating HEX file"
#)

# LEFT or MASTER
add_executable(${PROJECT_NAME}_master.elf ${MASTER_SOURCES})
target_compile_definitions(${PROJECT_NAME}_master.elf PRIVATE LEFT)
# set_target_properties(${PROJECT_NAME}_master.elf PROPERTIES OUTPUT_NAME master)

# RIGHT or SLAVE
add_executable(${PROJECT_NAME}_slave.elf ${SLAVE_SOURCES})
target_compile_definitions(${PROJECT_NAME}_slave.elf PRIVATE RIGHT)
# set_target_properties(${PROJECT_NAME}_slave.elf PROPERTIES OUTPUT_NAME slave)

# EEPROM
add_custom_command(TARGET ${PROJECT_NAME}_master.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 ${PROJECT_NAME}_master.elf ${PROJECT_NAME}_master.eep
	COMMENT "Generating *MASTER* EEPROM file"
)

add_custom_command(TARGET ${PROJECT_NAME}_slave.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0 ${PROJECT_NAME}_slave.elf ${PROJECT_NAME}_slave.eep
	COMMENT "Generating *SLAVE* EEPROM file"
)

# HEX
add_custom_command(TARGET ${PROJECT_NAME}_master.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${PROJECT_NAME}_master.elf ${PROJECT_NAME}_master.hex
	COMMENT "Generating *MASTER* HEX file"
)

add_custom_command(TARGET ${PROJECT_NAME}_slave.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${PROJECT_NAME}_slave.elf ${PROJECT_NAME}_slave.hex
	COMMENT "Generating *SLAVE* HEX file"
)