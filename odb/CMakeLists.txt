cmake_minimum_required(VERSION 3.17)

set(Target odb)

project(${Target} VERSION 1.3.2)

# 加载基础模板
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/base.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/odb.cmake)

# 生成config.in
string(TIMESTAMP COMPILE_TIME %Y%m%d%H%M%S)
set(COMPILE_TYPE ${CMAKE_BUILD_TYPE})
set(TARGET_OS ${CMAKE_SYSTEM_NAME} ${TargetArch})
set(COMPILE_TYPE ${CMAKE_BUILD_TYPE})
set(SOFTWARE_VERSION ${WITH_VERSION}-${COMPILE_TIME})

configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# conan依赖加载
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(${target})

# c++版本
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
add_definitions(-DKESTREL_STATIC=1)

# 声明变量
set(CUDA_PATH_V11_7 "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.7")

# 头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CUDA_PATH_V11_7}/include)


# 依赖库搜索路径
link_directories(${CUDA_PATH_V11_7}/lib/x64)

set(TARGET_LINK_LIBS ${TARGET_LINK_LIBS} ${CONAN_LIBS})

# ODB文件生成
odb_generate(dbtarget "DBTarget")
odb_generate(dbtracker "DBTracker")


# 添加头文件
add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加源文件
#add_source_recursively(${CMAKE_CURRENT_SOURCE_DIR}/src/local)

#set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/src/logic.cpp
#           ${CMAKE_CURRENT_SOURCE_DIR}/src/logictype.cpp)

# 添加conanfile
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py)
# 添加config.h
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_LIST_DIR}/config.h.in)

# for access
set(SOURCE ${SOURCE}
           ${CMAKE_CURRENT_SOURCE_DIR}/include/db_access.h)
set(SOURCE ${SOURCE}
          ${CMAKE_CURRENT_SOURCE_DIR}/include/db_operate.h)
set(SOURCE ${SOURCE}
           ${CMAKE_CURRENT_SOURCE_DIR}/src/db_operate.cpp)
#set(SOURCE
#    ${SOURCE}
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/db_operate.h)
#set(SOURCE
#    ${SOURCE}
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/db_operate.cpp)

# 指定生成对象依赖的源文件
add_library(${Target} SHARED ${SOURCE})
add_executable(${Target}-main ${SOURCE} ${ODB_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)
# 指定生成对象的依赖库
target_link_libraries(${Target} ${TARGET_LINK_LIBS})
target_link_libraries(${Target}-main ${TARGET_LINK_LIBS})

if(WITH_TEST)
  add_subdirectory(test)
endif()

# 指定安装路径
install(
  TARGETS ${Target}
  DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/lib
)
install(
  TARGETS ${Target}-main
  DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/bin
)
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/
)
