cmake_minimum_required(VERSION 3.17)
# 声明目标文件
set(TargetApp backbone_main)

# 项目版本
project(${TargetApp} VERSION 1.0)

# conan依赖加载
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(${TargetApp})

# 头文件搜索路径
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/include/extension)

# 依赖库搜索路径
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/bin)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/extension)

# 指定源文件
set(APP_SOURCE ${APP_SOURCE} main.cpp ${extensionsHeader})

# 指定生成对象依赖的源文件
add_executable(${TargetApp} ${APP_SOURCE} )

# 指定生成对象的依赖库
target_link_libraries(${TargetApp} PRIVATE ${TARGET_LINK_LIBS} ${Target} )

# 安装
install(
    TARGETS ${TargetApp}
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/demo
)
