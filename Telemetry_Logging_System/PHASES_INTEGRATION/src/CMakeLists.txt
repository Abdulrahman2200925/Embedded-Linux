## cmake in src directory

# =============================================================================
# Generated CommonAPI source files
# These are produced by the generators and must be compiled
# =============================================================================
set(GENERATED_SOURCES
    ${CMAKE_SOURCE_DIR}/src/generated/v1/omnimetron/gpu/GpuUsageDataSomeIPDeployment.cpp
    ${CMAKE_SOURCE_DIR}/src/generated/v1/omnimetron/gpu/GpuUsageDataSomeIPProxy.cpp
    ${CMAKE_SOURCE_DIR}/src/generated/v1/omnimetron/gpu/GpuUsageDataSomeIPStubAdapter.cpp
)

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

    # Phase 3 files
    LogManagerBuilder.cpp
    LogSinkFactory.cpp
    SocketSinkImpl.cpp

    # Phase 4 files
    ThreadPool.cpp

    # Bonus files
    SystemTelemetryWriter.cpp

    # Phase 5 files — raw vsomeip (Step 1)
    SomeIPTelemetrySourceImpl.cpp

    # Phase 5 files — CommonAPI generated (Mega Bonus)
    ${GENERATED_SOURCES}

    # Phase 5 files — CommonAPI service impl (Mega Bonus)
    GpuServiceImpl.cpp

    TelemetrySystem.cpp
)

target_include_directories(logmanager_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/generated
        ${CMAKE_SOURCE_DIR}/src/generated/v1
        ${CMAKE_SOURCE_DIR}/src/generated/v1/omnimetron/gpu
)

# =============================================================================
# Link all required libraries
#
# vsomeip3       — raw vsomeip (still needed underneath CommonAPI)
# CommonAPI      — CommonAPI core runtime
# CommonAPI-SomeIP — SomeIP binding for CommonAPI
# =============================================================================
target_link_libraries(logmanager_lib
    PUBLIC
        vsomeip3
        CommonAPI
        CommonAPI-SomeIP
)