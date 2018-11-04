project(basics CXX)

add_executable(${PROJECT_NAME} client.cpp)

target_link_libraries(${PROJECT_NAME} pthread)