cmake_minimum_required(VERSION 3.17)

# 声明目标文件
set(Target extension)

# 项目版本
project(${Target} VERSION 1.0)

# 加载基础模板
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/base.cmake)

# 生成config.in
string(TIMESTAMP COMPILE_TIME %Y%m%d%H%M%S)
set(COMPILE_TYPE ${CMAKE_BUILD_TYPE})
set(TARGET_OS ${CMAKE_SYSTEM_NAME} ${TargetArch})
set(COMPILE_TYPE ${CMAKE_BUILD_TYPE})
set(SOFTWARE_VERSION v${WITH_VERSION}-${COMPILE_TIME})

configure_file("${CMAKE_CURRENT_LIST_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# conan依赖加载
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(${target})

# 添加编译宏
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(EXTENSION_DLL_EXPORTS)

# 头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# 指定依赖库
set(TARGET_LINK_LIBS ${TARGET_LINK_LIBS} ${CONAN_LIBS})

if(UNIX)
    set(TARGET_LINK_LIBS ${TARGET_LINK_LIBS} dl)
endif()

# 添加头文件
# add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# 添加源文件
add_header_recursively(${CMAKE_CURRENT_SOURCE_DIR}/src/)
add_source_recursively(${CMAKE_CURRENT_SOURCE_DIR}/src/)

# 添加conanfile.py
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py)

# 添加config.h.in
set(SOURCE ${SOURCE} ${CMAKE_CURRENT_LIST_DIR}/config.h.in)

# 指定生成对象依赖的源文件
add_library(${Target} SHARED ${SOURCE})
# add_library(lib${Target} STATIC ${SOURCE})

target_link_libraries(${Target} ${TARGET_LINK_LIBS})

# 指定安装
install(
    TARGETS ${Target}
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/lib
)

install(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/extension.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/extension-hub.h
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/include
)

if(WITH_EXTENSION)
    add_subdirectory(extensions)
endif()

if(WITH_DOC)
   set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
   add_subdirectory(docs)
endif()

if(WITH_TEST)
    add_subdirectory(test)
endif()

if(WITH_APP)
   add_subdirectory(app)
endif()
