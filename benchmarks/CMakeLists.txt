include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        "origin/main"
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

include(FetchContent)
FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        "origin/main"
)

FetchContent_MakeAvailable(googlebenchmark)
#target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(benchmark_tests benchmark_main.cpp response_test.cpp)

target_link_libraries(benchmark_tests PRIVATE benchmark::benchmark server)