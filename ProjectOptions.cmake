include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

macro(simpleserv_supports_sanitizers)
	if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
	set(SUPPORTS_UBSAN ON)
	else()
	set(SUPPORTS_UBSAN OFF)
	endif()

	if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
	set(SUPPORTS_ASAN OFF)
	else()
	set(SUPPORTS_ASAN ON)
	endif()
endmacro()

macro(simpleserv_setup_options)
  option(simpleserv_ENABLE_HARDENING "Enable hardening" ON)
  option(simpleserv_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    simpleserv_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    simpleserv_ENABLE_HARDENING
    OFF)

  simpleserv_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR simpleserv_PACKAGING_MAINTAINER_MODE)
    option(simpleserv_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(simpleserv_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(simpleserv_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(simpleserv_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(simpleserv_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(simpleserv_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(simpleserv_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(simpleserv_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(simpleserv_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(simpleserv_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
		option(simpleserv_ENABLE_INCLUDE_WHAT_YOU_USE "Enable include-what-you-use" OFF)
    option(simpleserv_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(simpleserv_ENABLE_PCH "Enable precompiled headers" OFF)
    option(simpleserv_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(simpleserv_ENABLE_IPO "Enable IPO/LTO" ON)
    option(simpleserv_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(simpleserv_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(simpleserv_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(simpleserv_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(simpleserv_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(simpleserv_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(simpleserv_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(simpleserv_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(simpleserv_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
		option(simpleserv_ENABLE_INCLUDE_WHAT_YOU_USE "Enable include-what-you-use" ON)
    option(simpleserv_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(simpleserv_ENABLE_PCH "Enable precompiled headers" OFF)
    option(simpleserv_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      simpleserv_ENABLE_IPO
      simpleserv_WARNINGS_AS_ERRORS
      simpleserv_ENABLE_USER_LINKER
      simpleserv_ENABLE_SANITIZER_ADDRESS
      simpleserv_ENABLE_SANITIZER_LEAK
      simpleserv_ENABLE_SANITIZER_UNDEFINED
      simpleserv_ENABLE_SANITIZER_THREAD
      simpleserv_ENABLE_SANITIZER_MEMORY
      simpleserv_ENABLE_UNITY_BUILD
      simpleserv_ENABLE_CLANG_TIDY
			simpleserv_ENABLE_INCLUDE_WHAT_YOU_USE
      simpleserv_ENABLE_CPPCHECK
      simpleserv_ENABLE_COVERAGE
      simpleserv_ENABLE_PCH
      simpleserv_ENABLE_CACHE)
  endif()

  simpleserv_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (simpleserv_ENABLE_SANITIZER_ADDRESS OR simpleserv_ENABLE_SANITIZER_THREAD OR simpleserv_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(simpleserv_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(simpleserv_global_options)
  if(simpleserv_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    simpleserv_enable_ipo()
  endif()

  simpleserv_supports_sanitizers()

  if(simpleserv_ENABLE_HARDENING AND simpleserv_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR simpleserv_ENABLE_SANITIZER_UNDEFINED
       OR simpleserv_ENABLE_SANITIZER_ADDRESS
       OR simpleserv_ENABLE_SANITIZER_THREAD
       OR simpleserv_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${simpleserv_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${simpleserv_ENABLE_SANITIZER_UNDEFINED}")
    simpleserv_enable_hardening(simpleserv_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(simpleserv_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(simpleserv_warnings INTERFACE)
  add_library(simpleserv_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  simpleserv_set_project_warnings(
    simpleserv_warnings
    ${simpleserv_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(simpleserv_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(simpleserv_options)
  endif()

  include(cmake/Sanitizers.cmake)
  simpleserv_enable_sanitizers(
    simpleserv_options
    ${simpleserv_ENABLE_SANITIZER_ADDRESS}
    ${simpleserv_ENABLE_SANITIZER_LEAK}
    ${simpleserv_ENABLE_SANITIZER_UNDEFINED}
    ${simpleserv_ENABLE_SANITIZER_THREAD}
    ${simpleserv_ENABLE_SANITIZER_MEMORY})

  set_target_properties(simpleserv_options PROPERTIES UNITY_BUILD ${simpleserv_ENABLE_UNITY_BUILD})

  if(simpleserv_ENABLE_PCH)
    target_precompile_headers(
      simpleserv_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(simpleserv_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    simpleserv_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(simpleserv_ENABLE_CLANG_TIDY)
    simpleserv_enable_clang_tidy(simpleserv_options ${simpleserv_WARNINGS_AS_ERRORS})
  endif()

	if(simpleserv_ENABLE_INCLUDE_WHAT_YOU_USE)
		simpleserv_enable_include_what_you_use(simpleserv_options ${simpleserv_WARNINGS_AS_ERRORS})
	endif()

  if(simpleserv_ENABLE_CPPCHECK)
    simpleserv_enable_cppcheck(${simpleserv_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(simpleserv_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    simpleserv_enable_coverage(simpleserv_options)
  endif()

  if(simpleserv_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(simpleserv_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(simpleserv_ENABLE_HARDENING AND NOT simpleserv_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR simpleserv_ENABLE_SANITIZER_UNDEFINED
       OR simpleserv_ENABLE_SANITIZER_ADDRESS
       OR simpleserv_ENABLE_SANITIZER_THREAD
       OR simpleserv_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    simpleserv_enable_hardening(simpleserv_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()