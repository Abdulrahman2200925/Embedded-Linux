## root cmake
cmake_minimum_required(VERSION 3.16.0)

project(LogManager 
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

add_subdirectory(src)
add_subdirectory(app)
#add_subdirectory(tests)
