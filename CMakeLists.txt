cmake_minimum_required(VERSION 3.0)
project(crpc)

set(CMAKE_BUILD_TYPE "Debug")

# 可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/examples)
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party)

# 库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
add_subdirectory(examples)