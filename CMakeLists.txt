cmake_minimum_required(VERSION 3.25)

if (NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 23)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Simpleserv
	VERSION 0.0.1
	DESCRIPTION ""
	LANGUAGES CXX)	
set(CMAKE_BUILD_TYPE Release)

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

simpleserv_setup_options()

simpleserv_global_options()
include(Dependencies.cmake)
simpleserv_setup_dependencies()

simpleserv_local_options()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

target_compile_features(simpleserv_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(simpleserv::simpleserv_options ALIAS simpleserv_options)
add_library(simpleserv::simpleserv_warnings ALIAS simpleserv_warnings)

# configure files based on CMake configuration options
# add_subdirectory(configured_files)

# Adding the src:
add_subdirectory(src)

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# TODO test

# add_subdirectory(benchmarks)
# add_subdirectory(src)
# add_subdirectory(tests)

# # 

# # ###############################################################################
# # ## file globbing ##############################################################
# # ###############################################################################

# # # these instructions search the directory tree when CMake is
# # # invoked and put all files that match the pattern in the variables
# # # `sources` and `data`
# # file(GLOB_RECURSE sources      src/main/*.cpp src/main/*.hpp)
# # file(GLOB_RECURSE sources_test src/test/*.cpp)
# # file(GLOB_RECURSE data resources/*)
# # # you can use set(sources src/main.cpp) etc if you don't want to
# # # use globbing to find files automatically

# # ###############################################################################
# # ## target definitions #########################################################
# # ###############################################################################

# # # add the data to the target, so it becomes visible in some IDE
# # add_executable(example ${sources} ${data})

# # # just for example add some compiler flags
# target_compile_options(server PRIVATE 
# -fdiagnostics-color

# -Wfloat-conversion
# -Wall
# -Wextra
# -Wpedantic
# -Wuninitialized
# -Wmissing-include-dirs
# -Wshadow
# -Wundef
# -Winvalid-pch
# -Wnon-virtual-dtor
# -pedantic
# -Wimplicit-fallthrough

# -Winit-self
# -Wswitch-enum -Wswitch-default
# -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k

# -Wdouble-promotion
# -Wfloat-equal
# -Wpointer-arith


# -Wstrict-overflow=5
# -Wcast-qual
# -Wcast-align
# -Wconversion
# -Wsign-conversion
# -Wpacked


# -Wstrict-aliasing -fstrict-aliasing
# -Wredundant-decls
# -Wmissing-declarations
# -Wmissing-field-initializers

# -Wwrite-strings
# -Wstack-protector -fstack-protector
# #-Wpadded
# -Winline
# -Wdisabled-optimization


# -Wzero-as-null-pointer-constant
# -Wctor-dtor-privacy
# -Wold-style-cast
# -Woverloaded-virtual


# -Wlogical-op
# -Wstack-usage=1024 -fstack-usage -Wframe-larger-than=1024
# -Wtrampolines
# -Wvector-operation-performance


# -Wuseless-cast
# -Wnoexcept
# -Wstrict-null-sentinel


# -Wsuggest-final-types
# -Wsuggest-final-methods
# -Wsuggest-override


# -Wshift-overflow
# -Wshift-negative-value
# -Wnull-dereference
# -Wshift-overflow=2
# -Wduplicated-cond

# -Wmisleading-indentation

# #-Wvirtual-inheritance
# #-Wtemplates
# #-Wmultiple-inheritance


# -Wunused-macros
# -Wstringop-overflow=4
# -Wduplicated-branches
# -Walloc-zero
# -Walloca


# -Wcast-align=strict
# -Wstringop-truncation
# -Wextra-semi

# -Wstrict-overflow=4

# -Wsuggest-attribute=pure
# -Wsuggest-attribute=const
# -Wsuggest-attribute=noreturn
# -Wmissing-noreturn
# )

# add_compile_definitions(
#   $<CONFIG>
# )

# # # this lets me include files relative to the root source directory with a <> pair
# # target_include_directories(example PUBLIC src/main)

# # # this copies all resource files in the build directory
# # # we need this, because we want to work with paths relative to the executable
# # file(COPY ${data} DESTINATION resources)

# # ###############################################################################
# # ## dependencies ###############################################################
# # ###############################################################################

# # # this defines the variables Boost_LIBRARIES that contain all library names
# # # that we need to link to
# # #find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)

# # #target_link_libraries(example PUBLIC
# # #  ${Boost_LIBRARIES}
# #   # here you can add any library dependencies
# # #)

# # ###############################################################################
# # ## testing ####################################################################
# # ###############################################################################

# # this is for our testing framework
# # we don't add REQUIRED because it's just for testing
# # find_package(GTest)

# # if(GTEST_FOUND)
# #   add_executable(unit_tests ${sources_test} ${sources})

# #   # we add this define to prevent collision with the main
# #   # this might be better solved by not adding the source with the main to the
# #   # testing target
# #   target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

# #   # this allows us to use our executable as a link library
# #   # therefore we can inherit all compiler options and library dependencies
# #   set_target_properties(example PROPERTIES ENABLE_EXPORTS on)

# #   target_link_libraries(unit_tests PUBLIC
# #     ${GTEST_BOTH_LIBRARIES}
# #     server_lib
# #   )

# #   target_include_directories(unit_tests PUBLIC
# #     ${GTEST_INCLUDE_DIRS} # doesn't do anything on Linux
# #   )
# # endif()