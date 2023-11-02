cmake_minimum_required(VERSION 3.26)
project(VigilSDKTest)

set(CMAKE_CXX_STANDARD 23)

find_package(doctest)

add_executable(VigilSDKTest UnitTest.cpp
        UnitTest.h)

target_link_libraries(VigilSDKTest doctest::doctest)
