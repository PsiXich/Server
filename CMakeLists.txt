cmake_minimum_required(VERSION 3.15)
project(MyClientProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

find_package(Boost 1.70 REQUIRED COMPONENTS system)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR)
endif()

add_executable(server
    server.cpp
    client.cpp
)
target_link_libraries(server PRIVATE
    Boost::system
    ws2_32
    mswsock
)