macro(PROTO_GENERATE proto)
  set(grpcpbccFile
      "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.grpc.pb.cc"
  )
  set(grpcpbhFile
      "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.grpc.pb.h"
  )
  set(pbccFile
      "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.pb.cc"
  )
  set(pbhFile
    "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.pb.h"
  )

  set(PROTO_SOURCES ${PROTO_SOURCES} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.grpc.pb.cc")
  set(PROTO_SOURCES ${PROTO_SOURCES} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.grpc.pb.h")
  set(PROTO_SOURCES ${PROTO_SOURCES} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.pb.cc")
  set(PROTO_SOURCES ${PROTO_SOURCES} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.pb.h")

  set(PROTO_TARGET "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.grpc.pb.cc")
  set(PROTO_TARGET ${PROTO_TARGET} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.grpc.pb.h")
  set(PROTO_TARGET ${PROTO_TARGET} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.pb.cc")
  set(PROTO_TARGET ${PROTO_TARGET} "${CMAKE_SOURCE_DIR}/src/proto/${proto}/${proto}.pb.h")
    add_custom_command(
       OUTPUT ${PROTO_TARGET}
       PRE_BUILD
       COMMAND ${Protobuf_PROTOC_EXECUTABLE} --version&&${Protobuf_PROTOC_EXECUTABLE} --proto_path=${CMAKE_SOURCE_DIR}/proto/ --cpp_out=${CMAKE_SOURCE_DIR}/src/proto/${proto}/ --grpc_out=${CMAKE_SOURCE_DIR}/src/proto/${proto}/
       --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_PROGRAM} -I
       ${CMAKE_SOURCE_DIR} ${proto}.proto
       COMMENT "Generate grpc proto support code for
       ${proto}.h")

endmacro()