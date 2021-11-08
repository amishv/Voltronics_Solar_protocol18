cmake_minimum_required(VERSION 3.0.0)
project(solarmon VERSION 0.1.0)

include(CTest)
enable_testing()
#Bring the headers, s into the project
include_directories(include)
#Can manually add the sources using the set command as follows:
#set(SOURCES src/solarmon.c src/qpigs.c src/qpiri.c src/communication.c src/qmisc.c) 
#the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.c")
add_executable(solarmon ${SOURCES})
target_link_libraries (solarmon paho-mqtt3c) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)