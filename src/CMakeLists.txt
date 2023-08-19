
add_library(serverlib parser.cpp server.cpp http.cpp url.cpp)
target_include_directories(serverlib PUBLIC include)

add_executable(server main.cpp)
#target_sources(server PRIVATE parser.cpp server.cpp http.cpp  url.cpp)

target_link_libraries(server serverlib)