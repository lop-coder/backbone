cmake_minimum_required(VERSION 3.1)
# 声明目标文件
set(TargetTest Test)

# 项目版本
project(${TargetTest} VERSION 1.0)

# 加载基础模板
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/base.cmake)

add_executable(${TargetTest} main.cpp)
# 加载googletest模板,需放到最后
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/test.cmake)
