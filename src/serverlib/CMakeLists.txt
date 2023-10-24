include(GenerateExportHeader)

add_library(serverlib content_coding.cpp file_manager.cpp message.cpp request_handler.cpp url.cpp http/parser.cpp)

add_library(simpleserv::serverlib ALIAS serverlib)

target_link_libraries(serverlib PRIVATE simpleserv_options simpleserv_warnings)

target_link_libraries(serverlib PUBLIC wolfssl::wolfssl libzippp::libzippp brotlienc ${Boost_LIBRARIES})

target_include_directories(serverlib ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(serverlib PUBLIC cxx_std_23)

set_target_properties(
  serverlib
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(serverlib EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/myproject/serverlib_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(serverlib PUBLIC SERVER_LIB_STATIC_DEFINE)
endif()