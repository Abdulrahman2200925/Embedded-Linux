## root cmake
cmake_minimum_required(VERSION 3.16.0)

project(LogManager 
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# Find vsomeip3
# =============================================================================
find_package(vsomeip3 REQUIRED)

# =============================================================================
# Find CommonAPI Core runtime
# =============================================================================
find_package(CommonAPI REQUIRED)

# =============================================================================
# Find CommonAPI SomeIP binding
# =============================================================================
find_package(CommonAPI-SomeIP REQUIRED)

# =============================================================================
# Add generated files include path globally
# =============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/src/generated
    ${CMAKE_SOURCE_DIR}/include
)

# Enable testing
enable_testing()

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tools)
#add_subdirectory(tests)