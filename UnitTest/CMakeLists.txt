cmake_minimum_required(VERSION 3.26)
project(VigilSDKTest)

set(CMAKE_CXX_STANDARD 20)

find_package(doctest)
find_package(nlohmann_json)

add_executable(VigilSDKTest UnitTest.cpp
        UnitTest.h)

target_link_libraries(VigilSDKTest VigilSDK doctest::doctest nlohmann_json::nlohmann_json)
