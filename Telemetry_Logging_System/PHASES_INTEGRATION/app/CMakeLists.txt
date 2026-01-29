## cmake in app directory
add_executable(app main.cpp)

target_link_libraries(app PRIVATE logmanager_lib)
