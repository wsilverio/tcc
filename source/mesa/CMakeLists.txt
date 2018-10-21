cmake_minimum_required(VERSION 3.12)
project(mesa)

include_directories(./include)
include_directories(~/workspace/Sming/Sming)
include_directories(~/workspace/Sming/Sming/Libraries)
include_directories(~/workspace/Sming/Sming/system/include)
include_directories(~/workspace/esp-open-sdk/sdk/include)

set(SOURCE_FILES app/application.cpp include/plomodefs.h include/plomotypes.h include/hardware.h app/hardware.cpp include/sensors.h app/sensors.cpp include/matrix.h app/matrix.cpp include/uart.h app/uart.cpp include/tabmask8.h)
add_executable(mesa ${SOURCE_FILES})
