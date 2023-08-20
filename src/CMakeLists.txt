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

find_package(WolfSSL REQUIRED)

add_library(serverlib parser.cpp request_handler.cpp server.cpp http.cpp url.cpp)
target_include_directories(serverlib PUBLIC include)

target_link_libraries(serverlib PRIVATE WolfSSL::WolfSSL)
add_executable(server main.cpp)

#target_sources(server PRIVATE parser.cpp server.cpp http.cpp  url.cpp)

target_link_libraries(server serverlib)