cmake_minimum_required(VERSION 3.21)
project(fp_growth)

set(CMAKE_CXX_STANDARD 14)


# set(DATA_FILE "${CMAKE_SOURCE_DIR}/chainstoreFIM.txt")
set(DATA_FILE "${CMAKE_SOURCE_DIR}/OnlineRetailZZ.txt")
set(OUT_FILE "${CMAKE_SOURCE_DIR}/result.txt")

add_definitions(-DDATA_FILE="${DATA_FILE}")
add_definitions(-DOUT_FILE="${OUT_FILE}")

add_executable(fp_growth fp_growth.cpp)
