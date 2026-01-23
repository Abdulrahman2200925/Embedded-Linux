# Find Google Test (includes Google Mock)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# All library sources
set(LIB_SOURCES
    ${CMAKE_SOURCE_DIR}/src/LogMessage.cpp
    ${CMAKE_SOURCE_DIR}/src/LogManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ConsoleSinkImpl.cpp
    ${CMAKE_SOURCE_DIR}/src/FileSinkImpl.cpp
    ${CMAKE_SOURCE_DIR}/src/SafeSocket.cpp
    ${CMAKE_SOURCE_DIR}/src/SocketSinkImpl.cpp
    ${CMAKE_SOURCE_DIR}/src/SafeFile.cpp
    ${CMAKE_SOURCE_DIR}/src/LogSinkFactory.cpp
    ${CMAKE_SOURCE_DIR}/src/LogManagerBuilder.cpp
    ${CMAKE_SOURCE_DIR}/src/FileTelemetrySourceImpl.cpp
    ${CMAKE_SOURCE_DIR}/src/SocketTelemetrySourceImpl.cpp
    ${CMAKE_SOURCE_DIR}/src/SystemTelemetryWriter.cpp
)

# Test 1: LogManager Tests
add_executable(test_logmanager 
    ${CMAKE_CURRENT_SOURCE_DIR}/LogManger_test.cpp 
    ${LIB_SOURCES}
)
target_link_libraries(test_logmanager
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME LogManager_Tests COMMAND test_logmanager)


# Test 2: SafeFile Tests
add_executable(test_safefile 
    ${CMAKE_CURRENT_SOURCE_DIR}/SafeFile_test.cpp 
    ${LIB_SOURCES}
)
target_link_libraries(test_safefile
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME SafeFile_Tests COMMAND test_safefile)

# Test 3: SafeSocket Tests
add_executable(test_safesocket 
    ${CMAKE_CURRENT_SOURCE_DIR}/SafeSocket_test.cpp 
    ${LIB_SOURCES}
)
target_link_libraries(test_safesocket
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME SafeSocket_Tests COMMAND test_safesocket)

# Test 4: FileTelemetrySource Tests
add_executable(test_filetelemetry 
    ${CMAKE_CURRENT_SOURCE_DIR}/FileTelemetrySourceImpl_test.cpp 
    ${LIB_SOURCES}
)

target_link_libraries(test_filetelemetry
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME FileTelemetry_Tests COMMAND test_filetelemetry)

# Test 5: SocketTelemetrySource Tests
add_executable(test_sockettelemetry 
    ${CMAKE_CURRENT_SOURCE_DIR}/SocketTelemetrySourceImpl_test.cpp 
    ${LIB_SOURCES}
)
target_link_libraries(test_sockettelemetry
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME SocketTelemetry_Tests COMMAND test_sockettelemetry)

# Test 6: Phase3 Tests
add_executable(test_phase3 
    ${CMAKE_CURRENT_SOURCE_DIR}/phase3_tests.cpp 
    ${LIB_SOURCES}
)
target_link_libraries(test_phase3
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME Phase3_Tests COMMAND test_phase3)

# Test 7: Integration Tests
add_executable(test_integration 
    ${CMAKE_CURRENT_SOURCE_DIR}/integration_test.cpp 
    ${LIB_SOURCES}
)
target_link_libraries(test_integration
  PRIVATE
    GTest::gmock_main
    GTest::gtest
    Threads::Threads
)
add_test(NAME Integration_Tests COMMAND test_integration)