cmake_minimum_required(VERSION 3.14)
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR avr)
SET(CMAKE_C_STANDARD 99)

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#============================================================================================
project(HP_Clock_Clion C)
SET(DEVICE atmega328p)
SET(FREQ 16000000)
#============================================================================================
SET(FILES
        main.c
        Libs/RTC.h
        Libs/RTC.c
        Libs/control.h
        Libs/control.c
        Libs/DHT11.c Libs/DHT11.h)

SET(FLASH NO CACHE BOOL "Flash")
#============================================================================================
SET(AVRBIN "C:/WinAVR/bin")
SET(AVROBJCOPY avr-objcopy)
SET(AVRSIZE avr-size)
SET(AVRDUDE avrdude)

SET(PROGRAMMER avrispmkii)
SET(PORT usb)
#============================================================================================
SET(CMAKE_C_COMPILER ${AVRBIN}/avr-gcc.exe)
SET(CMAKE_CXX_COMPILER ${AVRBIN}/avr-g++.exe)

SET(CMAKE_C_FLAGS "-Os -mmcu=${DEVICE} -DF_CPU=${FREQ}UL -std=gnu99 -Wl,--gc-sections")
SET(CMAKE_CXX_FLAGS "-Os -mmcu=${DEVICE} -DF_CPU=${FREQ}UL -Wl,--gc-sections")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
SET(CMAKE_EXECUTABLE_SUFFIX ".elf")
SET_PROPERTY(DIRECTORY PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.eep"
)
INCLUDE_DIRECTORIES("C:/WinAVR/avr/include")
#============================================================================================
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${FILES})

ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVROBJCOPY} -O ihex -R .eeprom ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.elf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex)

ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVRSIZE} --mcu=${DEVICE} -C --target=ihex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.elf)

if(FLASH)
    ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD COMMAND ${AVRBIN}/${AVRDUDE} -p${DEVICE} -c${PROGRAMMER} -P${PORT} -U flash:w:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex:i)
endif(FLASH)