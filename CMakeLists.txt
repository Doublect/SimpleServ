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

# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  message(AUTHOR_WARNING "Building Tests.")
  add_subdirectory(tests)
endif()

if(myproject_BUILD_FUZZ_TESTS)
  message(AUTHOR_WARNING "Building Fuzz Tests, using fuzzing sanitizer https://www.llvm.org/docs/LibFuzzer.html")
  if (NOT myproject_ENABLE_ADDRESS_SANITIZER AND NOT myproject_ENABLE_THREAD_SANITIZER)
    message(WARNING "You need asan or tsan enabled for meaningful fuzz testing")
  endif()
  add_subdirectory(fuzz_test)
endif()

# If MSVC is being used, and ASAN is enabled, we need to set the debugger environment
# so that it behaves well with MSVC's debugger, and we can run the target from visual studio
if(MSVC)
  get_all_installable_targets(all_targets)
  message("all_targets=${all_targets}")
  set_target_properties(${all_targets} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%")
endif()

# set the startup project for the "play" button in MSVC
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT intro)

if(CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

include(cmake/PackageProject.cmake)

# Add other targets that you want installed here, by default we just package the one executable
# we know we want to ship
myproject_package_project(
  TARGETS
  serverexec
  simpleserv_options
  simpleserv_warnings
  # FIXME: this does not work! CK
  # PRIVATE_DEPENDENCIES_CONFIGURED project_options project_warnings
)

# Experience shows that explicit package naming can help make it easier to sort
# out potential ABI related issues before they start, while helping you
# track a build to a specific GIT SHA
set(CPACK_PACKAGE_FILE_NAME
    "${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${GIT_SHORT_SHA}-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}-${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}"
)

include(CPack)


# TODO: readd compile warning flags 
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
