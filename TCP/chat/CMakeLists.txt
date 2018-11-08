project(chat CXX)

set(ClientExe "${PROJECT_NAME}Client")
set(ServerExe "${PROJECT_NAME}Server")

add_executable(${ClientExe} clientMain.cpp)
target_link_libraries(${ClientExe} pthread)

add_executable(${ServerExe} serverMain.cpp)
target_link_libraries(${ServerExe} pthread)
