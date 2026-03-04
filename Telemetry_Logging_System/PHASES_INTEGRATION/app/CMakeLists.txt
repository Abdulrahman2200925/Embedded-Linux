## cmake in app directory
add_executable(app main.cpp)

target_link_libraries(app
    PRIVATE
        -Wl,--whole-archive logmanager_lib -Wl,--no-whole-archive
        vsomeip3
        CommonAPI
        CommonAPI-SomeIP
)
