project(server CXX)

add_executable(${PROJECT_NAME}_sync sync_server.cpp)
target_link_libraries(${PROJECT_NAME}_sync pthread)
