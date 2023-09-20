find_path(WOLFSSL_INCLUDE_DIR NAMES wolfssl/ssl.h PATHS /usr/include)
find_library(WOLFSSL_LIBRARY NAMES libwolfssl.so PATHS /usr/lib)
if (WOLFSSL_INCLUDE_DIR)
    set(WOLFSSL_FOUND TRUE)
endif()

if (WOLFSSL_FOUND AND WOLFSSL_LIBRARY)
    if (NOT TARGET WolfSSL::WolfSSL)
        add_library(WolfSSL::WolfSSL INTERFACE IMPORTED)
        set_target_properties(WolfSSL::WolfSSL PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${WOLFSSL_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${WOLFSSL_LIBRARY}"
        )
    endif()
else()
    message(STATUS "WolfSSL library not found.")
endif()

# find_path(BROTLI_INCLUDE_DIR NAMES decode.h encode.h port.h types.h PATHS /usr/include/brotli)
# find_library(BROTLI_LIBRARY NAMES libbrotlicommon.so libbrotlidec.so libbrotlienc.so PATHS /usr/lib)
# if (BROTLI_INCLUDE_DIR)
#     set(BROTLI_FOUND TRUE)
# endif()

# add_library(Brotli::Brotli INTERFACE IMPORTED)
#         set_target_properties(Brotli::Brotli PROPERTIES
#             INTERFACE_INCLUDE_DIRECTORIES "${BROTLI_INCLUDE_DIR}"
#             INTERFACE_LINK_LIBRARIES "${BROTLI_LIBRARY}"
#         )
# if (BROTLI_FOUND AND BROTLI_LIBRARY)
#     if (NOT TARGET Brotli::Brotli)
        
#     endif()
# else()
#     message(STATUS "Brotli library not found.")
# endif()

find_package(WolfSSL REQUIRED)
#find_package(Brotli REQUIRED)

include(FetchContent)
FetchContent_Declare(
  libzippp	
  GIT_REPOSITORY https://github.com/ctabin/libzippp.git
  GIT_TAG        libzippp-v6.1-1.9.2
)
FetchContent_MakeAvailable(libzippp)

include(FetchContent)
FetchContent_Declare(
  brotli	
  GIT_REPOSITORY https://github.com/google/brotli
  GIT_TAG        v1.0.9
)
FetchContent_MakeAvailable(brotli)

add_library(serverlib SHARED)
target_link_libraries(serverlib PUBLIC WolfSSL::WolfSSL libzippp::libzippp brotlienc)
target_include_directories(serverlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_sources(serverlib PUBLIC content_coding.cpp file_manager.cpp parser.cpp request_handler.cpp server.cpp http.cpp url.cpp)

add_executable(server main.cpp)
target_link_libraries(server PRIVATE serverlib)