## cmake in src directory

add_library(logmanager_lib
    # Phase 1 files
    ConsoleSinkImpl.cpp
    FileSinkImpl.cpp
    LogManager.cpp
    LogMessage.cpp
    
    # Phase 2 files
    SafeFile.cpp
    FileTelemetrySourceImpl.cpp
    SafeSocket.cpp
    SocketTelemetrySourceImpl.cpp

    #Phase 3 files
    LogManagerBuilder.cpp
    LogSinkFactory.cpp
    SocketSinkImpl.cpp

    #Phase 4 files
    ThreadPool.cpp

    # Bonus files (if you have them)

    SystemTelemetryWriter.cpp
)

target_include_directories(logmanager_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
)