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
		
	# 	if(wolfssl_ADDED)
	# 		add_library(GTest::GTest INTERFACE IMPORTED)
	# 		target_link_libraries(GTest::GTest INTERFACE gtest_main)
	# 	else()
	# 		message(STATUS "GTest library not found.")
	# 	endif()
	# endif()

	if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()
endfunction()