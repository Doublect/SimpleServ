include(cmake/CPM.cmake)

function(simpleserv_setup_dependencies)
	if(NOT TARGET brotlienc)
		cpmaddpackage("gh:google/brotli#v1.1.0")
	endif()
	
	if(NOT TARGET libzippp::libzippp)
		cpmaddpackage("gh:ctabin/libzippp#libzippp-v7.0-1.10.1")
	endif()

	if(NOT TARGET wolfssl::wolfssl)
		cpmaddpackage(
			NAME wolfssl
			GITHUB_REPOSITORY wolfSSL/wolfssl
			GIT_TAG v5.6.3-stable)
		
		if(wolfssl_ADDED)
			add_library(WolfSSL::WolfSSL INTERFACE IMPORTED)
			set_target_properties(WolfSSL::WolfSSL PROPERTIES
					INTERFACE_INCLUDE_DIRECTORIES "${WOLFSSL_INCLUDE_DIR}"
					INTERFACE_LINK_LIBRARIES "${WOLFSSL_LIBRARY}"
			)
		else()
			message(STATUS "WolfSSL library not found.")
		endif()
		# find_path(WOLFSSL_INCLUDE_DIR NAMES wolfssl/ssl.h PATHS /usr/include)
		# find_library(WOLFSSL_LIBRARY NAMES libwolfssl.so PATHS /usr/lib)
		# if (WOLFSSL_INCLUDE_DIR)
		# 	set(WOLFSSL_FOUND TRUE)
		# endif()

		# if (WOLFSSL_FOUND AND WOLFSSL_LIBRARY)
		# 	add_library(wolfssl::wolfssl INTERFACE IMPORTED)
		# 	set_target_properties(wolfssl::wolfssl PROPERTIES
		# 			INTERFACE_INCLUDE_DIRECTORIES "${WOLFSSL_INCLUDE_DIR}"
		# 			INTERFACE_LINK_LIBRARIES "${WOLFSSL_LIBRARY}"
		# 	)
		# else()
		# 	message(STATUS "WolfSSL library not found.")
		# endif()
	endif()

	# if(NOT TARGET GTest::GTest)
	# 	cpmaddpackage(
	# 		NAME GTest
	# 		GITHUB_REPOSITORY google/googletest
	# 		GIT_TAG origin/main)
		
	# 	if(GTest_ADDED)
	# 		add_library(GTest::GTest INTERFACE IMPORTED)
	# 		target_link_libraries(GTest::GTest INTERFACE gtest_main)
	# 	else()
	# 		message(STATUS "GTest library not found.")
	# 	endif()
	# endif()

	if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()

	if(NOT TARGET Boost)
		set(Boost_USE_STATIC_LIBS OFF) 
		set(Boost_USE_MULTITHREADED ON)  
		set(Boost_USE_STATIC_RUNTIME OFF) 
		#find_package(Boost 1.83 COMPONENTS asio)

		if(Boost_FOUND)
				include_directories(${Boost_INCLUDE_DIRS}) 
		else()
		# TODO: configure boost, when no local installation is present
			cpmaddpackage(
				NAME Boost
				VERSION 1.83.0
				GITHUB_REPOSITORY "boostorg/boost"
				GIT_TAG "boost-1.83.0"
			)
		endif()
	endif()
		
endfunction()