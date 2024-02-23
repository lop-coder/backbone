set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/../include)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake" ${CMAKE_MODULE_PATH})

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/docs.cmake)

# 安装
install(
    DIRECTORY ${SPHINX_BUILD}/
    DESTINATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${TargetArch}/${Target}/doc
)
