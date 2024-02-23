macro(ODB_GENERATE header schema)
  set(cxxFile
      "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype-odb.cxx"
  )
  set(hxxFile
      "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype-odb.hxx"
  )
  set(ixxFile
      "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype-odb.ixx"
  )
  # SET(sqlFile
  # "${CMAKE_SOURCE_DIR}/local/odbormtype/${header}/${header}ormtype-odb.sql")

  set(ODB_GENERATED_FILES ${cxxFile} ${hxxFile} ${ixxFile})

  set(ODB_SOURCES
      ${ODB_SOURCES}
      "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype-odb.cxx"
  )
  set(ODB_SOURCES
      ${ODB_SOURCES}
      "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype-odb.hxx"
  )

  set(ODB_SOURCES
        ${ODB_SOURCES}
        "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype.h"
    )
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype.cpp")
        set(ODB_SOURCES
              ${ODB_SOURCES}
              "${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype.cpp"
          )
    endif()
  # set(ODB_SOURCES ${ODB_SOURCES}
  # "${CMAKE_SOURCE_DIR}/src/local/odbormtype/${header}/${header}ormtype.h")
  #if(NOT EXISTS "${CMAKE_SOURCE_DIR}/odbormtype/${header}/${header}ormtype-odb.hxx" OR  NOT EXISTS "${CMAKE_SOURCE_DIR}/odbormtype/${header}/${header}ormtype-odb.cxx" )
  #    if(${schema} MATCHES "")
  #      execute_process(
  #        COMMAND
  #          ${CONAN_BIN_DIRS_LIBODB}/odb.exe --std c++11 --database sqlite
  #          --generate-query --generate-schema --schema-name ${schema} -I
  #          ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON} -I ${CONAN_INCLUDE_DIRS_LIBODB} -I
  #          ${CMAKE_SOURCE_DIR} ${header}ormtype.h
  #        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/odbormtype/${header}/)
  #    else()
  #      execute_process(
  #        COMMAND
  #          ${CONAN_BIN_DIRS_LIBODB}/odb.exe --std c++11 --database sqlite
  #          --generate-query --generate-schema -I
  #          ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON} -I ${CONAN_INCLUDE_DIRS_LIBODB} -I
  #          ${CMAKE_SOURCE_DIR} ${header}ormtype.h
  #        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/odbormtype/${header}/)
  #    endif()
  #  endif()
    if(${schema} MATCHES "")
        add_custom_command(
            OUTPUT ${ODB_GENERATED_FILES}
            PRE_BUILD COMMAND cd ${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/&&${CONAN_BIN_DIRS_LIBODB}/odb.exe
            --std c++11 --database sqlite --generate-query  --generate-session --generate-schema
            --schema-name ${schema} -I ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON} -I
            ${CONAN_INCLUDE_DIRS_LIBODB} -I ../../ ${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype.h
            COMMENT "Generate database support code for ${header}.h")
    else()
        add_custom_command(
            OUTPUT ${ODB_GENERATED_FILES} PRE_BUILD
            COMMAND cd ${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/&&${CONAN_BIN_DIRS_LIBODB}/odb.exe
            --std c++11 --database sqlite --generate-query --generate-session --generate-schema -I
            ${CONAN_INCLUDE_DIRS_NLOHMANN_JSON} -I ${CONAN_INCLUDE_DIRS_LIBODB} -I
            ../../ ${CMAKE_SOURCE_DIR}/src/odbormtype/${header}/${header}ormtype.h
            COMMENT "Generate database support code for ${header}.h")
    endif()
endmacro()



