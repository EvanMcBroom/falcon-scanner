include(FetchContent)

# Do not use FetchContent to pin repos to a specific version
# The git submodules they reference are already pinned to a version
FetchContent_Declare(magic_enum SOURCE_DIR ${PROJECT_SOURCE_DIR}/libraries/magic_enum)
FetchContent_Declare(pybind11 SOURCE_DIR "${PROJECT_SOURCE_DIR}/libraries/pybind11")

# Default single-configuration generators to Release while configuring pybind11.
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release) 
  set(BUILD_TYPE_SET TRUE) 
endif()
FetchContent_MakeAvailable(magic_enum pybind11)
if(DEFINED BUILD_TYPE_SET)
  unset(CMAKE_BUILD_TYPE)
  unset(BUILD_TYPE_SET)
endif()
