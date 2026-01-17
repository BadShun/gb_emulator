cmake_minimum_required(VERSION 3.10.0)
project(gb_emulator VERSION 0.1.0 LANGUAGES C)
# 设置 C 标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
# 头文件路径
include_directories(${PROJECT_SOURCE_DIR}/include)
# 源文件
file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.c
)
# 生成可执行文件
add_executable(gb_emulator ${SRC_FILES})

