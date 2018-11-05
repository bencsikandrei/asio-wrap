project(http CXX)

add_executable(${PROJECT_NAME} http_connect.cpp)
target_link_libraries(${PROJECT_NAME} pthread)
