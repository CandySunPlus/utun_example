cmake_minimum_required(VERSION 3.2)
project(utun)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src DIR_SRCS)
add_executable(${PROJECT_NAME} ${DIR_SRCS})
