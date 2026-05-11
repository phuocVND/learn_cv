cmake_minimum_required(VERSION 3.20)
project(OmniVisionEdge VERSION 0.1.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Build settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ---------------------------------------------------------------------------
# Find dependencies
# ---------------------------------------------------------------------------
find_package(OpenCV 4 REQUIRED COMPONENTS core imgproc videoio highgui)
find_package(Boost 1.80 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

# Optional: spdlog (Milestone 5)
find_package(spdlog QUIET)

# ---------------------------------------------------------------------------
# Header-only interface library (Milestone 1-4 skeleton headers)
# ---------------------------------------------------------------------------
add_library(omnivision_headers INTERFACE)
target_include_directories(omnivision_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(omnivision_headers INTERFACE
    ${OpenCV_LIBS}
    Boost::system
    Threads::Threads
)
if(spdlog_FOUND)
    target_link_libraries(omnivision_headers INTERFACE spdlog::spdlog)
    target_compile_definitions(omnivision_headers INTERFACE OMNIVISION_HAVE_SPDLOG)
endif()

# ---------------------------------------------------------------------------
# Main executable (bạn thêm src/*.cpp khi implement từng milestone)
# ---------------------------------------------------------------------------
# add_executable(omnivision_edge
#     src/main.cpp
#     src/network_manager.cpp   # Milestone 1
#     src/frame_buffer.cpp      # Milestone 2
#     src/processor.cpp         # Milestone 3
#     src/app_controller.cpp    # Milestone 5
# )
# target_link_libraries(omnivision_edge PRIVATE omnivision_headers)

# ---------------------------------------------------------------------------
# Unit tests (bạn thêm khi cần)
# ---------------------------------------------------------------------------
# add_executable(test_ring_buffer tests/test_ring_buffer.cpp)
# target_link_libraries(test_ring_buffer PRIVATE omnivision_headers)

# ---------------------------------------------------------------------------
# Compiler flags
# ---------------------------------------------------------------------------
add_compile_options(
    -Wall -Wextra -Wpedantic
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-march=native>   # enable SIMD (NEON on Apple Silicon, AVX2 on x86)
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)
add_link_options(
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)
