cmake_minimum_required(VERSION 3.17)
# 声明目标文件
set(Target logger)

# 项目版本
project(${Target} VERSION 1.0.1.0)

# 加载基础模板
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/base.cmake)

message("project name: ${PROJECT_NAME}")
message("cmake project name: ${CMAKE_PROJECT_NAME}")
message("project version: ${PROJECT_VERSION}")

# 打印编译环境
string(TIMESTAMP COMPILE_TIME %Y%m%d%H%M%S)
set(COMPILE_TYPE ${CMAKE_BUILD_TYPE})
set(TARGET_OS ${CMAKE_SYSTEM_NAME} ${TargetArch})
set(COMPILE_TYPE ${CMAKE_BUILD_TYPE})
set(SOFTWARE_VERSION v${PROJECT_VERSION}-${COMPILE_TIME})

configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# c++版本
set(CMAKE_CXX_STANDARD 17)

# fmt
find_package(fmt REQUIRED)
include_directories(${fmt_INCLUDE_DIR})

# nlohmann_json
find_package(nlohmann_json REQUIRED)
include_directories(${nlohmann_json_INCLUDE_DIRS})

# spdlog
find_package(spdlog REQUIRED)
include_directories(${spdlog_INCLUDE_DIR})


# LOGGER_DLL_EXPORTS
add_compile_definitions(LOGGER_DLL_EXPORTS)

add_compile_definitions(SPDLOG_FMT_EXTERNAL)
add_compile_definitions(_HAS_AUTO_PTR_ETC=1)

# LEAK_DETECTOR
#set (LEAK_DETECTOR_CHECK 10)
if(LEAK_DETECTOR_CHECK)
add_compile_definitions(LEAK_DETECTOR=10)
endif()

# 声明变量
if(LEAK_DETECTOR_CHECK)
set(VLD_PATH "C:/Program Files (x86)/Visual Leak Detector")
endif()

# 头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(LEAK_DETECTOR_CHECK)
include_directories(${VLD_PATH}/include)
endif()

# 依赖库搜索路径
if(LEAK_DETECTOR_CHECK)
link_directories(${VLD_PATH}/bin/Win64)
link_directories(${VLD_PATH}/lib/Win64)
endif()

# 指定依赖库
if(LEAK_DETECTOR_CHECK)
set(TARGET_LINK_LIBS
    ${TARGET_LINK_LIBS}
    vld)
endif()

# 添加头文件
#add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/include)
#add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/src/backward.hpp)

# 添加源文件
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp)
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/src/backward_wrapper.cpp)



# 添加conanfile
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py)
# 添加config.h
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_LIST_DIR}/config.h.in)

# 指定生成对象依赖的源文件
add_library(${Target} SHARED ${SOURCE})

add_executable(${Target}-demo  ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp )

# 指定生成对象的依赖库
target_link_libraries(${Target} PRIVATE ${spdlog_LIBRARIES} ${fmt_LIBRARIES}  ${nlohmann_json_LIBRARIES})

target_link_libraries(${Target}-demo PRIVATE ${Target}
    ${spdlog_LIBRARIES}
    ${fmt_LIBRARIES}
    ${nlohmann_json_LIBRARIES})

# 指定安装路径
install(
  TARGETS ${Target}
  DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/bin
)

install(
  TARGETS ${Target}-demo
  DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/bin
)

# 添加子路径
if(WITH_TEST)
  add_subdirectory(test)
endif()
