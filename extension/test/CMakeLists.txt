cmake_minimum_required(VERSION 3.17)
# 声明目标文件
set(TargetTest Test)

# 项目版本
project(${TargetTest} VERSION 1.0)

# Find all extensionTest-*.cpp
file(GLOB_RECURSE extensionsTest RELATIVE "${CMKAE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/extensionTest-*.cpp")
foreach(extension IN LISTS extensionsTest)
    get_filename_component(extensionTestName ${extension} NAME_WE)
    #get_filename_component(extensionPath ${extension} DIRECTORY)
    string(REGEX REPLACE "extension-" "" result ${extensionTestName})
    string(STRIP ${result} extensionTestName)
    add_executable(${extensionTestName} ${extension} ${SOURCE})
    target_link_libraries(${extensionTestName} gtest  ${TARGET_LINK_LIBS})
    install(
        TARGETS ${extensionTestName}
        DESTINATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/test/extension
    )
endforeach()

# 生成对象
add_executable(${TargetTest} ${SOURCE}   main.cpp factoryTest.cpp extensionHubTest.cpp )
add_executable(echoWorker ${SOURCE}  echoworker.cpp )
install(
    TARGETS ${TargetTest}
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/test
)

# 加载googletest模板,需放到最后
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/test.cmake)

# 链接对应库
target_link_libraries(${TargetTest} ${TARGET_LINK_LIBS})
target_link_libraries(echoWorker ${TARGET_LINK_LIBS})
