cmake_minimum_required(VERSION 3.15)

# set the project name
project(blackcat_vs_bluecat)

add_compile_definitions(LIBEIGEN3_DEV)
add_compile_definitions(MY_DEBUG)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14")
set(source_dir "${PROJECT_SOURCE_DIR}/src/cpp")

file (GLOB source_files "${source_dir}/*.cpp")

# add the executable
add_executable(${PROJECT_NAME} ${source_files})