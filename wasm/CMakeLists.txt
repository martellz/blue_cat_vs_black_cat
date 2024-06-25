cmake_minimum_required(VERSION 3.15)

# set the project name
project(blackcat_vs_bluecat)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(LIBEIGEN3_DEV)
# add_compile_definitions(MY_DEBUG)

# if you want to debug, remove "-O3"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++17 -O3")
set(source_dir "${PROJECT_SOURCE_DIR}/src/cpp")

# python
find_package(PythonLibs REQUIRED)

# for matplotlib-cpp
## change the path to your own python path
set(PYTHON_INCLUDE_DIRS "/home/zyf/miniconda3/envs/cppEnv/include/python3.10")
set(PYTHON_LIBRARIES "/home/zyf/miniconda3/envs/cppEnv/lib/libpython3.10.so")
include_directories(${PYTHON_INCLUDE_DIRS})
include_directories("/home/zyf/miniconda3/envs/cppEnv/lib/python3.10/site-packages/numpy/_core/include")

## change the path to your own matplotlib-cpp path
include_directories("/home/zyf/cpp_packages/matplotlib-cpp")

file (GLOB source_files "${source_dir}/*.cpp")

# add the executable
add_executable(${PROJECT_NAME} ${source_files})

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES})
