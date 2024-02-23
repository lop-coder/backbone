cmake_minimum_required(VERSION 3.1)
# 声明目标文件
set(TargetTest Test)

# 项目版本
project(${TargetTest} VERSION 1.0)


# 遍历添加测试文件
file(GLOB_RECURSE sources RELATIVE "${CMKAE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# 生成对象
add_executable(${TargetTest} ${SOURCE} ${ODB_SOURCES} ${sources})

# 加载googletest模板,需放到最后
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/test.cmake)

# 链接对应库
target_link_libraries(${TargetTest} ${TARGET_LINK_LIBS})


install(
    TARGETS ${TargetTest}
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/test
)
