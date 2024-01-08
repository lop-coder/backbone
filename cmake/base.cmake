set(archdetect_c_code
    "
#if defined(__arm__) || defined(__TARGET_ARCH_ARM)
    #if defined(__ARM_ARCH_7__) \\
        || defined(__ARM_ARCH_7A__) \\
        || defined(__ARM_ARCH_7R__) \\
        || defined(__ARM_ARCH_7M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
        #error cmake_ARCH armv7
    #elif defined(__ARM_ARCH_6__) \\
        || defined(__ARM_ARCH_6J__) \\
        || defined(__ARM_ARCH_6T2__) \\
        || defined(__ARM_ARCH_6Z__) \\
        || defined(__ARM_ARCH_6K__) \\
        || defined(__ARM_ARCH_6ZK__) \\
        || defined(__ARM_ARCH_6M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
        #error cmake_ARCH armv6
    #elif defined(__ARM_ARCH_5TEJ__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
        #error cmake_ARCH armv5
    #else
        #error cmake_ARCH arm
    #endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #error cmake_ARCH i386
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #error cmake_ARCH x86_64
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
    #error cmake_ARCH ia64
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \\
      || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \\
      || defined(_M_MPPC) || defined(_M_PPC)
    #if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
        #error cmake_ARCH ppc64
    #else
        #error cmake_ARCH ppc
    #endif
#endif

#error cmake_ARCH unknown
")

function(TARGET_ARCHITECTURE)
  if(APPLE AND CMAKE_OSX_ARCHITECTURES)
    foreach(osx_arch ${CMAKE_OSX_ARCHITECTURES})
      if("${osx_arch}" STREQUAL "ppc" AND ppc_support)
        set(osx_arch_ppc TRUE)
      elseif("${osx_arch}" STREQUAL "i386")
        set(osx_arch_i386 TRUE)
      elseif("${osx_arch}" STREQUAL "x86_64")
        set(osx_arch_x86_64 TRUE)
      elseif("${osx_arch}" STREQUAL "ppc64" AND ppc_support)
        set(osx_arch_ppc64 TRUE)
      else()
        message(FATAL_ERROR "Invalid OS X arch name: ${osx_arch}")
      endif()
    endforeach()
    if(osx_arch_ppc)
      list(APPEND ARCH ppc)
    endif()

    if(osx_arch_i386)
      list(APPEND ARCH i386)
    endif()

    if(osx_arch_x86_64)
      list(APPEND ARCH x86_64)
    endif()

    if(osx_arch_ppc64)
      list(APPEND ARCH ppc64)
    endif()
  else()
    file(WRITE "${CMAKE_BINARY_DIR}/arch.c" "${archdetect_c_code}")
    enable_language(C)
    try_run(
      run_result_unused compile_result_unused "${CMAKE_BINARY_DIR}"
      "${CMAKE_BINARY_DIR}/arch.c"
      COMPILE_OUTPUT_VARIABLE ARCH
      CMAKE_FLAGS CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES})
    string(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH "${ARCH}")
    string(REPLACE "cmake_ARCH " "" ARCH "${ARCH}")
    if(NOT ARCH)
      set(ARCH unknown)
    endif()
  endif()
  set(TargetArch
      "${ARCH}"
      PARENT_SCOPE)
endfunction()

function(target_force_link_libraries target lib)
  if(MSVC)
    set_target_properties(${target} PROPERTIES LINK_FLAGS
                                               "/WHOLEARCHIVE:${lib}")
  else()
    set_target_properties(
      ${target}
      PROPERTIES LINK_FLAGS
                 "-Wl,--whole-archive lib${lib}.a -Wl,--no-whole-archive")
  endif()
endfunction()

macro(ADD_HEADER_RECURSIVELY root_dir)
  if(IS_DIRECTORY ${root_dir})
    file(
      GLOB Headers
      RELATIVE ${root_dir}
      ${root_dir}/*.h ${root_dir}/*.hpp)
    foreach(header ${Headers})
      set(SOURCE ${SOURCE} "${root_dir}/${header}")
    endforeach()
  endif()

  file(
    GLOB ALL_SUB
    RELATIVE ${root_dir}
    ${root_dir}/*)

  foreach(sub ${ALL_SUB})
    if(IS_DIRECTORY ${root_dir}/${sub})
      file(
        GLOB Headers
        RELATIVE ${root_dir}/${sub}
        ${root_dir}/${sub}/*.h ${root_dir}/${sub}/*.hpp)
      foreach(header ${Headers})
        set(SOURCE ${SOURCE} "${root_dir}/${sub}/${header}")
      endforeach()
      add_header_recursively(${root_dir}/${sub})
    endif()
  endforeach()
endmacro()

macro(ADD_CPP_RECURSIVELY root_dir)
  if(IS_DIRECTORY ${root_dir})
    aux_source_directory(${root_dir} SRCS)
    set(SOURCE ${SOURCE} "${SRCS}")
  endif()

  file(
    GLOB ALL_SUB
    RELATIVE ${root_dir}
    ${root_dir}/*)

  foreach(sub ${ALL_SUB})
    if(IS_DIRECTORY ${root_dir}/${sub})
      aux_source_directory(${root_dir}/${sub} SRCS)
      set(SOURCE ${SOURCE} "${SRCS}")
      add_cpp_recursively(${root_dir}/${sub})
    endif()
  endforeach()
endmacro()

macro(ADD_SOURCE_RECURSIVELY root_dir)
  if(IS_DIRECTORY ${root_dir})
    file(
      GLOB Sources
      RELATIVE ${root_dir}
      ${root_dir}/*.cpp ${root_dir}/*.h ${root_dir}/*.hpp)
    foreach(source ${Sources})
      set(SOURCE ${SOURCE} "${root_dir}/${source}")
    endforeach()
  endif()

  file(
    GLOB ALL_SUB
    RELATIVE ${root_dir}
    ${root_dir}/*)

  foreach(sub ${ALL_SUB})
    if(IS_DIRECTORY ${root_dir}/${sub})
      file(
        GLOB Sources
        RELATIVE ${root_dir}/${sub}
        ${root_dir}/${sub}/*.cpp ${root_dir}/${sub}/*.h
        ${root_dir}/${sub}/*.hpp)
      foreach(source ${Sources})
        set(SOURCE ${SOURCE} "${root_dir}/${sub}/${source}")
      endforeach()
      add_source_recursively(${root_dir}/${sub})
    endif()
  endforeach()
endmacro()

macro(ADD_FILE_RECURSIVELY root_dir)
  file(
    GLOB ALL_SUB
    RELATIVE ${root_dir}
    ${root_dir}/*)
  foreach(sub ${ALL_SUB})
    if(IS_DIRECTORY ${root_dir}/${sub})
      add_file_recursively(${root_dir}/${sub})
    else()
      set(SOURCE ${SOURCE} "${root_dir}/${sub}")
    endif()
  endforeach()
endmacro()

macro(ODB_GENERATE header schema)
  set(cxxFile
      "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.cxx"
  )
  set(hxxFile
      "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.hxx"
  )
  set(ixxFile
      "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.ixx"
  )
  # SET(sqlFile
  # "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.sql")

  set(ODB_GENERATED_FILES ${cxxFile} ${hxxFile} ${ixxFile})

  set(ODB_SOURCES
      ${ODB_SOURCES}
      "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.cxx"
  )
  set(ODB_SOURCES
      ${ODB_SOURCES}
      "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.hxx"
  )

  # set(ODB_SOURCES ${ODB_SOURCES}
  # "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype.h")
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.hxx" OR  NOT EXISTS "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype-odb.cxx" )
    if(${schema} MATCHES "")
        execute_process(
          COMMAND
          ${CONAN_BIN_DIRS_LIBODB}/odb.exe
          --std c++11 --database sqlite --generate-query --generate-schema
          --schema-name ${schema} -I ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON} -I
          ${CONAN_INCLUDE_DIRS_LIBODB} -I ../../../.. ${header}ormtype.h
          WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/
          )
    else()
        execute_process(
          COMMAND
          ${CONAN_BIN_DIRS_LIBODB}/odb.exe
          --std c++11 --database sqlite --generate-query --generate-schema
           -I ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON} -I
          ${CONAN_INCLUDE_DIRS_LIBODB} -I ../../../.. ${header}ormtype.h
          WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/
          )
    endif()
    endif()
endmacro()

macro(DO_TEST Target arg1 arg2 result)
  add_test(test_${arg1}_${arg2} Target ${arg1} ${arg2})
  set_tests_properties(test_${arg1}_${arg2} PROPERTIES PASS_REGULAR_EXPRESSION
                                                       ${result})
endmacro(DO_TEST)

enable_testing()

add_compile_definitions("ISDEBUG=$<CONFIG:Debug>")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_architecture()
  message(STATUS "----- OS: Windows " ${TargetArch})
  if(CMAKE_CL_64)
    message(STATUS "----- MSVC 64bit")
  else()
    message(STATUS "----- MSVC 32bit")
  endif()

  if(MSVC_VERSION EQUAL 1600)
    set(vs_version vs2010)
    set(vc_version vc10)
  elseif(MSVC_VERSION EQUAL 1700)
    set(vs_version vs2012)
    set(vc_version vc11)
  elseif(MSVC_VERSION EQUAL 1800)
    set(vs_version vs2013)
    set(vc_version vc12)
  elseif(MSVC_VERSION EQUAL 1900)
    set(vs_version vs2015)
    set(vc_version vc14)
  elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS_EQUAL 1920)
    set(vs_version vs2017)
    set(vc_version vc15)
  elseif(MSVC_VERSION GREATER_EQUAL 1920)
    set(vs_version vs2019)
    set(vc_version vc15)
  endif()
  message(STATUS "----- vs_version is: ${vs_version}")
  message(STATUS "----- vc_version is: ${vc_version}")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_architecture()
  message(STATUS "----- OS: Linux " ${TargetArch})
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  target_architecture()
  message(STATUS "----- OS: MacOS X " ${TargetArch})
elseif(CMAKE_SYSTEM_NAME MATCHES "Android")
  target_architecture()
  message(STATUS "----- OS: Android " ${TargetArch})
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug"
   OR CMAKE_BUILD_TYPE EQUAL "None"
   OR NOT CMAKE_BUILD_TYPE)
  message(STATUS "----- CMAKE_BUILD_TYPE is Debug")
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
  message(STATUS "----- CMAKE_BUILD_TYPE is Release")
elseif(CMAKE_BUILD_TYPE MATCHES "RelWitchDebInfo")
  message(STATUS "----- CMAKE_BUILD_TYPE is RelWitchDebInfo")
elseif(CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
  message(STATUS "----- CMAKE_BUILD_TYPE is MinSizeRel")
else()
  message(STATUS "----- unknown CMAKE_BUILD_TYPE = " ${CMAKE_BUILD_TYPE})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
  add_compile_options(
    "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/utf-8;/bigobj;/Zc:preprocessor;/wd4624;/wd4005;/wd5105>"
  )
  add_link_options("$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/ignore:4099>")
  add_link_options(
    "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:$<$<CONFIG:Debug>:/INCREMENTAL>>")
else()
  link_libraries(
    "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:-lstdc++fs>"
  )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,GNU>:--coverage>")
    add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-O1>")
    add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,GNU>:-g>")
    link_libraries("gcov")
  endif()
endif(MSVC)
