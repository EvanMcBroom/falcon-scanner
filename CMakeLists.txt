cmake_minimum_required(VERSION 3.21.0)
project(falcon-scanner
    VERSION "0.1"
    DESCRIPTION "A library and tools for interacting with Falcon extensions"
    HOMEPAGE_URL "https://github.com/SpecterOps/falcon-scanner"
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(requirements.cmake)

add_library(falcon)
target_sources(falcon PUBLIC
    include/libaviary.hpp
    include/libfalcon.hpp
)
target_sources(falcon PRIVATE falcon.cpp)
target_include_directories(falcon PUBLIC include)

add_executable(scan_file)
target_sources(scan_file PRIVATE examples/scan_file.cpp)
target_link_libraries(scan_file PRIVATE falcon magic_enum::magic_enum)