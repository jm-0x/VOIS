cmake_minimum_required(VERSION 3.7...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION)
endif()

project(ChatApplication VERSION 1.0
                        DESCRIPTION  "A chat application"
                        LANGUAGES CXX)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()