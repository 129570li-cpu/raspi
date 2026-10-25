cmake_minimum_required(VERSION 3.16)

project(raspi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_CLIENT "Build Qt client" OFF)
option(BUILD_SERVER "Build Linux server" OFF)
option(BUILD_CHATROOM "Build chatroom" OFF)

if(BUILD_CHATROOM)
    add_subdirectory(chatroom)
endif()




