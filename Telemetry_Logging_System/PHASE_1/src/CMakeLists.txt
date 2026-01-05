add_library(logmanager_lib
    ConsoleSinkImpl.cpp
    FileSinkImpl.cpp
    LogManager.cpp
    LogMessage.cpp
)

target_include_directories(logmanager_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
)