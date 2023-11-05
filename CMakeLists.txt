cmake_minimum_required(VERSION 3.26)
project(VigilSDK)

set(CMAKE_CXX_STANDARD 23)

include_directories(Source)

add_library(VigilSDK Source/Common/Base/BaseDefs.h
        Source/Vigil.cpp
        Source/Vigil.h
        Source/Common/System/Crc32.h
        Source/Common/ObjectModel/Object.h
        Source/Common/ObjectModel/Object.cpp
        Source/Common/ObjectModel/Reflection/ClassMember.h
        Source/Common/Container/Array.h
        Source/Common/ObjectModel/Reflection/ObjectReader.h
        Source/Common/ObjectModel/Reflection/Reader/JsonReader.h
        Source/Common/ObjectModel/Reflection/Reader/JsonReader.cpp
)

find_package(nlohmann_json)
target_link_libraries(VigilSDK nlohmann_json::nlohmann_json)

add_subdirectory(UnitTest)