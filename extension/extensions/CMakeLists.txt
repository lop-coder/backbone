cmake_minimum_required(VERSION 3.17)

# 声明目标文件
set(ExtensionTargets Extensions)

# 项目版本
project(${ExtensionTargets} VERSION 1.0)

add_compile_definitions(EXTENSION_DLL_EXPORTS)

# Find all extension-*.cpp files
file(GLOB_RECURSE extensionSources RELATIVE "${CMKAE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/extension-*.cpp")
foreach(extension IN LISTS extensionSources)
    get_filename_component(extensionName ${extension} NAME_WE)
    get_filename_component(extensionPath ${extension} DIRECTORY)
    string(REGEX REPLACE "extension-" "" result ${extensionName})
    string(STRIP ${result} extensionName)

    add_library(${extensionName} SHARED ${extension} ${extensionPath}/extension-${extensionName}.h )
    target_link_libraries(${extensionName} ${TARGET_LINK_LIBS})
    install(
        TARGETS ${extensionName}
        DESTINATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/extension
    )
install(
    FILES
    ${extensionPath}/extension-${extensionName}.h
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/include/extension
)
endforeach()




