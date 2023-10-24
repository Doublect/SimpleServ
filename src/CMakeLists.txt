add_subdirectory(serverlib)

add_subdirectory(server)

# # find_path(BROTLI_INCLUDE_DIR NAMES decode.h encode.h port.h types.h PATHS /usr/include/brotli)
# # find_library(BROTLI_LIBRARY NAMES libbrotlicommon.so libbrotlidec.so libbrotlienc.so PATHS /usr/lib)
# # if (BROTLI_INCLUDE_DIR)
# #     set(BROTLI_FOUND TRUE)
# # endif()

# # add_library(Brotli::Brotli INTERFACE IMPORTED)
# #         set_target_properties(Brotli::Brotli PROPERTIES
# #             INTERFACE_INCLUDE_DIRECTORIES "${BROTLI_INCLUDE_DIR}"
# #             INTERFACE_LINK_LIBRARIES "${BROTLI_LIBRARY}"
# #         )
# # if (BROTLI_FOUND AND BROTLI_LIBRARY)
# #     if (NOT TARGET Brotli::Brotli)
        
# #     endif()
# # else()
# #     message(STATUS "Brotli library not found.")
# # endif()

# set(Boost_USE_STATIC_LIBS OFF) 
# set(Boost_USE_MULTITHREADED ON)  
# set(Boost_USE_STATIC_RUNTIME OFF) 

# find_package(Boost 1.83.0 COMPONENTS asio)

# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS}) 
# else()
# 		message(STATUS "Boost library not found.")
# endif()

# find_package(WolfSSL REQUIRED)
# #find_package(Brotli REQUIRED)

# include(FetchContent)
# FetchContent_Declare(
#   libzippp	
#   GIT_REPOSITORY https://github.com/ctabin/libzippp.git
#   GIT_TAG        libzippp-v7.0-1.10.1
# )
# FetchContent_MakeAvailable(libzippp)

# include(FetchContent)
# FetchContent_Declare(
#   brotli	
#   GIT_REPOSITORY https://github.com/google/brotli
#   GIT_TAG        v1.1.0
# )
# FetchContent_MakeAvailable(brotli)

# add_compile_definitions(WOLFSSL_USER_SETTINGS)
# set(WOLFSSL_EXAMPLES OFF)
# set(WOLFSSL_CRYPT_TESTS OFF)
# set(WOLFSSL_USER_SETTINGS ON)

# file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/serverlib/*.hpp)
# add_library(server SHARED ${HEADERS})
# target_link_libraries(server PUBLIC WolfSSL::WolfSSL libzippp::libzippp brotlienc ${Boost_LIBRARIES})
# target_include_directories(server INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
# target_sources(server PUBLIC content_coding.cpp file_manager.cpp message.cpp request_handler.cpp url.cpp)

# add_executable(serverexec main.cpp)
# target_link_libraries(serverexec PRIVATE server)