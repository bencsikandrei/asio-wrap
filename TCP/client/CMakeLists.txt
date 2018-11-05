project(tcp_client CXX)

add_executable(${PROJECT_NAME}_sync sync_client.cpp)
target_link_libraries(${PROJECT_NAME}_sync pthread)

add_executable(${PROJECT_NAME}_async async_client.cpp)
target_link_libraries(${PROJECT_NAME}_async pthread)
