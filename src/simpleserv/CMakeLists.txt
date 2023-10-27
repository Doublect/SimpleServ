include(GenerateExportHeader)

file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/simpleserv/*.hpp)
add_library(simpleserv SHARED ${HEADERS})
# add_library(simpleserv content_coding.cpp file_manager.cpp request_handler.cpp url.cpp http/parser.cpp)

add_library(simpleserv::simpleserv ALIAS simpleserv)


target_link_libraries(simpleserv PRIVATE simpleserv_options simpleserv_warnings)

target_link_libraries(simpleserv PUBLIC wolfssl::wolfssl libzippp::libzippp brotlienc Boost::log)
# target_link_libraries(simpleserv PUBLIC wolfssl::wolfssl libzippp::libzippp brotlienc ${Boost_LIBRARIES})

# target_include_directories(simpleserv INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(simpleserv ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
																															$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(simpleserv PUBLIC cxx_std_23)
target_sources(simpleserv PUBLIC content_coding.cpp file_manager.cpp http/request_handler.cpp url.cpp http/parser.cpp)

set_target_properties(
  simpleserv
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(simpleserv EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/simpleserv/server_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(simpleserv PUBLIC SERVER_LIB_STATIC_DEFINE)
endif()