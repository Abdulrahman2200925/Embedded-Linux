set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(SYSROOT /home/mac/x-tools/aarch64-rpi3-linux-gnu/aarch64-rpi3-linux-gnu/sysroot)
set(VSOMEIP_BUILT /home/mac/vsomeip/vsomeip-built)
set(COMMONAPI_BUILT /home/mac/vsomeip/commonapi-built)
set(BOOST_BUILT /home/mac/vsomeip/boost-built)

set(CMAKE_C_COMPILER   /home/mac/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /home/mac/x-tools/aarch64-rpi3-linux-gnu/bin/aarch64-rpi3-linux-gnu-g++)

set(CMAKE_SYSROOT ${SYSROOT})
set(CMAKE_C_FLAGS   "--sysroot=${SYSROOT}")
set(CMAKE_CXX_FLAGS "--sysroot=${SYSROOT}")
# -L        : linker library search paths for explicitly requested libs (-l<name>)
# -rpath-link: paths ld.bfd searches when resolving DT_NEEDED entries of shared
#              libraries being linked (i.e. transitive deps). Without this the
#              cross linker cannot find libboost_thread.so needed by libvsomeip3.so.
set(_CROSS_LIB_FLAGS
    "-L${BOOST_BUILT}/lib -L${VSOMEIP_BUILT}/lib -L${COMMONAPI_BUILT}/lib"
    " -Wl,-rpath-link,${BOOST_BUILT}/lib"
    " -Wl,-rpath-link,${VSOMEIP_BUILT}/lib"
    " -Wl,-rpath-link,${COMMONAPI_BUILT}/lib"
)
string(CONCAT _CROSS_LIB_FLAGS ${_CROSS_LIB_FLAGS})
set(CMAKE_SHARED_LINKER_FLAGS "${_CROSS_LIB_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS    "${_CROSS_LIB_FLAGS}")

set(CMAKE_FIND_ROOT_PATH ${SYSROOT} ${VSOMEIP_BUILT} ${COMMONAPI_BUILT} ${BOOST_BUILT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
