cmake_minimum_required(VERSION 3.26)
project(VigilSDK)

set(CMAKE_CXX_STANDARD 23)

include_directories(Source)

add_library(VigilSDK Source/Common/Base/BaseDefs.h
        Source/Vigil.cpp
        Source/Vigil.h
        Source/Common/System/Crc32.h
        Source/Common/ObjectModel/Class.h
        Source/Common/ObjectModel/Object.h
)

option(UNIT_TESTING "Enable unit testing" OFF)
if(UNIT_TESTING)
    add_subdirectory(UnitTest)
endif()
