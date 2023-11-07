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

# # ###############################################################################
# # ## target definitions #########################################################
# # ###############################################################################

# # # add the data to the target, so it becomes visible in some IDE
# # add_executable(example ${sources} ${data})

# # # just for example add some compiler flags
# target_compile_options(simpleserv PRIVATE 
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
