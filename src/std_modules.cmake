set(STD_MODULES
    algorithm
    expected
    iostream
    iterator
    map
    memory
    sstream
    string
    stdexcept
    thread
    unordered_set
    vector
)

set(STD_MODULES_BUILD_TIMESTAMP_FILE std_modules_build_timestamp)

add_custom_target(std_modules ALL
    DEPENDS ${STD_MODULES_BUILD_TIMESTAMP_FILE}
)

set(STD_MODULES_OPTIONS -std=gnu++23 -c -x c++-system-header)

add_custom_command(
    OUTPUT ${STD_MODULES_BUILD_TIMESTAMP_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Building standard library modules"
    COMMAND ${CMAKE_CXX_COMPILER} ${STD_MODULES_OPTIONS} ${STD_MODULES}
    COMMAND cmake -E touch ${STD_MODULES_BUILD_TIMESTAMP_FILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    BYPRODUCTS
)