## cmake in tools directory

# =============================================================================
# mock_service — raw vsomeip mock (Step 1, kept for reference)
# =============================================================================
# add_executable(mock_service mock_service.cpp)

# target_include_directories(mock_service
#     PRIVATE
#         ${CMAKE_SOURCE_DIR}/include
# )

# target_link_libraries(mock_service
#     PRIVATE
#         vsomeip3
# )

# =============================================================================
# gpu_service — CommonAPI service (Mega Bonus)
# Uses generated GpuUsageDataStub via GpuServiceImpl
#
# To run:
#   VSOMEIP_CONFIGURATION=../../config/vsomeip-service.json \
#   COMMONAPI_CONFIG=../../config/commonapi-service.json \
#   ./tools/gpu_service
# =============================================================================
add_executable(gpu_service Gpu_service_main.cpp)

target_include_directories(gpu_service
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src/generated
        ${CMAKE_SOURCE_DIR}/src/generated/v1
        ${CMAKE_SOURCE_DIR}/src/generated/v1/omnimetron/gpu
)

target_link_libraries(gpu_service
    PRIVATE
        # --whole-archive forces the linker to include ALL object files from
        # logmanager_lib, including GpuUsageDataSomeIPStubAdapter.cpp.o whose
        # INITIALIZER macro registers the stub adapter factory with CommonAPI.
        # Without this, the linker strips that .o (no direct symbol references)
        # and runtime->registerService() returns false immediately.
        -Wl,--whole-archive logmanager_lib -Wl,--no-whole-archive
        vsomeip3
        CommonAPI
        CommonAPI-SomeIP
        # Explicitly pull in boost_thread so the cross linker resolves the
        # undefined references that come from vsomeip3's transitive dependency
        # on libboost_thread.so.1.74.0 (from /home/mac/vsomeip/boost-built/lib).
        boost_thread
)