add_executable(server main.cpp server.cpp http.cpp url.cpp)

add_library(serverlib main.cpp server.cpp http.cpp url.cpp)
target_include_directories(serverlib PUBLIC include)