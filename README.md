# SimpleServ - Simple HTTPS server

# Building SimpleServ

## Prerequisites
- An up-to-date C++ compiler (with C++23 support): 
    - GCC >12.0
    - Clang >16.0
- CMake >3.25

### Optional dependencies
SimpleServ uses [CPM](https://github.com/cpm-cmake/CPM.cmake) to manage dependencies. CPM will automatically download and build the dependencies when building SimpleServ. However, if you wish to use your own version of the following dependencies, you can install them manually.

- Boost 1.83.0

## Configuration
First, clone the SimpleServ repository:
```bash
git clone https://github.com/Doublect/SimpleServ.git
```

To configure the project, run the following commands:
```bash
mkdir build
cd build
cmake -G "Ninja Multi-Config" ../
```

To change variables, you can use the CMake GUI or edit the CMake cache.

## Building and Running

To build the project, run one of the following commands:
```bash
# Build for Debug
ninja -f build-Debug.ninja 
# Build for Release
ninja -f build-Release.ninja
```

The compiled executable will be in the `build/Debug` or `build/Release` directory. The default configuration uses the privilidged ports 80 and 443. To use these ports, the executable must be run as root, unless the binary is granted the `CAP_NET_BIND_SERVICE` capability (through `setcap`).

Currently, the server will server files from the relative directory `../webdir`. The program will exit if this directory does not exist.
```bash
# Run
sudo ./build/Debug/serverexec
# or
sudo ./build/Release/serverexec
```

## Testing

The project uses [Catch2](https://github.com/catchorg/Catch2) for unit testing. With the CMake `BUILD_TESTING` option enabled, the tests will be built alongside the project. To manually build and run the tests, run the following command:
```bash
ninja -f build-Debug.ninja test

./build/Debug/tests
```


# Credits
- [Boost](https://www.boost.org/)
- [Catch2](https://github.com/catchorg/Catch2)
- [CMake](https://cmake.org/)
- [CPM](https://github.com/cpm-cmake/CPM.cmake)
- [C++ Best Practices Template](https://github.com/cpp-best-practices/cmake_template) by Jason Turner and Contributors
- [WolfSSL](https://www.wolfssl.com/) ([Repository](https://github.com/wolfSSL/wolfssl]))
