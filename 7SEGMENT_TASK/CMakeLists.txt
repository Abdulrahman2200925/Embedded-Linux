cmake_minimum_required(VERSION 3.20)

project(SEGMENT LANGUAGES CXX ASM)

# ---------------------------
# Core library (all logic)
# ---------------------------
add_library(segment_core STATIC)

#add source files to the lib and to create object files .cpp->.o
target_sources(segment_core PRIVATE
    src/GPIO.cpp
    src/ISTREAM.cpp
    src/OSTREAM.cpp
    src/SEGMENT.cpp
    src/STREAM.cpp
)

#add include directories public to let the compiled sources see them
target_include_directories(segment_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# ---------------------------
# Executable (entry point)
# ---------------------------
# output in main.o
add_executable(SEGMENT
    app/main.cpp
)
# output in main.o + segment_core.a => elf file
target_link_libraries(SEGMENT PRIVATE segment_core)

