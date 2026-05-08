cmake_minimum_required(VERSION 3.20)
project(CV_BasicPipeline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(APPLE)
    message(STATUS "Platform: macOS")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        message(STATUS "Architecture: Apple Silicon (ARM64)")
    else()
        message(STATUS "Architecture: Intel")
    endif()
elseif(UNIX AND NOT APPLE)
    message(STATUS "Platform: Linux")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        message(STATUS "Architecture: x86_64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        message(STATUS "Architecture: ARM64")
    endif()
endif()

# Find OpenCV
find_package(OpenCV REQUIRED)
if(OpenCV_FOUND)
    message(STATUS "OpenCV found: ${OpenCV_VERSION}")
    message(STATUS "OpenCV libraries: ${OpenCV_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenCV not found. Install with: brew install opencv")
endif()

# Find Boost (header-only or specific components)
find_package(Boost REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found. Install with: brew install boost")
endif()
# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

# Create executable
add_executable(pipeline src/main.cpp)

# Link libraries
target_link_libraries(pipeline
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    pthread
)

# Compiler flags
if(APPLE)
    target_compile_options(pipeline PRIVATE -fcolor-diagnostics)
else()
    target_compile_options(pipeline PRIVATE -fdiagnostics-color=always)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Executable: ${CMAKE_BINARY_DIR}/pipeline")
