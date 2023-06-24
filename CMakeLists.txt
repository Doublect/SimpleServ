cmake_minimum_required(VERSION 3.0)
project(MyProject)
set(CMAKE_CXX_STANDARD 23)

#add_executable(mytest test1.cpp)

add_subdirectory(src)
add_subdirectory(tests)

# 

# ###############################################################################
# ## file globbing ##############################################################
# ###############################################################################

# # these instructions search the directory tree when CMake is
# # invoked and put all files that match the pattern in the variables
# # `sources` and `data`
# file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.hpp)
# file(GLOB_RECURSE sources_test src/test/*.cpp)
# file(GLOB_RECURSE data resources/*)
# # you can use set(sources src/main.cpp) etc if you don't want to
# # use globbing to find files automatically

# ###############################################################################
# ## target definitions #########################################################
# ###############################################################################

# # add the data to the target, so it becomes visible in some IDE
# add_executable(example ${sources} ${data})

# # just for example add some compiler flags
# target_compile_options(example PUBLIC -std=c++23c -Wall -Wfloat-conversion)

# # this lets me include files relative to the root source directory with a <> pair
# target_include_directories(example PUBLIC src/main)

# # this copies all resource files in the build directory
# # we need this, because we want to work with paths relative to the executable
# file(COPY ${data} DESTINATION resources)

# ###############################################################################
# ## dependencies ###############################################################
# ###############################################################################

# # this defines the variables Boost_LIBRARIES that contain all library names
# # that we need to link to
# #find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)

# #target_link_libraries(example PUBLIC
# #  ${Boost_LIBRARIES}
#   # here you can add any library dependencies
# #)

# ###############################################################################
# ## testing ####################################################################
# ###############################################################################

# this is for our testing framework
# we don't add REQUIRED because it's just for testing
# find_package(GTest)

# if(GTEST_FOUND)
#   add_executable(unit_tests ${sources_test} ${sources})

#   # we add this define to prevent collision with the main
#   # this might be better solved by not adding the source with the main to the
#   # testing target
#   target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

#   # this allows us to use our executable as a link library
#   # therefore we can inherit all compiler options and library dependencies
#   set_target_properties(example PROPERTIES ENABLE_EXPORTS on)

#   target_link_libraries(unit_tests PUBLIC
#     ${GTEST_BOTH_LIBRARIES}
#     server_lib
#   )

#   target_include_directories(unit_tests PUBLIC
#     ${GTEST_INCLUDE_DIRS} # doesn't do anything on Linux
#   )
# endif()