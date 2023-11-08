include(GenerateExportHeader)

add_library(simpleserv STATIC
	http/parser.cpp
	http/http_handler.cpp
	http/request_handler.cpp 
	utility/url.cpp 
	utility/content_coding.cpp 
	utility/file_manager.cpp
)

add_library(simpleserv::simpleserv ALIAS simpleserv)

target_link_libraries(simpleserv PRIVATE simpleserv_options simpleserv_warnings)

target_include_directories(simpleserv ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
																															$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_link_system_libraries(simpleserv PRIVATE wolfssl::wolfssl libzippp::libzippp brotlienc Boost::log)

target_compile_features(simpleserv PUBLIC cxx_std_23)

set_target_properties(
  simpleserv
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(simpleserv EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/simpleserv/server_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(simpleserv PUBLIC SERVER_LIB_STATIC_DEFINE)
endif()