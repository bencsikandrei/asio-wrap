cmake_minimum_required(VERSION 3.10)
project(asio CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -Wall -Wextra -DASIO_STANDALONE -g -O0")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    /home/andrei/ThirdParty/asio-standalone
)

add_subdirectory(TCP)
add_subdirectory(HTTP)
