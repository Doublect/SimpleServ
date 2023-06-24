add_executable(server main.cpp server.cpp http.cpp uri.cpp)

add_library(serverlib main.cpp server.cpp http.cpp uri.cpp)
target_include_directories(serverlib PUBLIC include)