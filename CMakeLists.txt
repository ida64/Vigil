cmake_minimum_required(VERSION 3.26)
project(VigilSDK)

set(CMAKE_CXX_STANDARD 20)

include_directories(Source)

add_library(VigilSDK Source/Common/Base/BaseDefs.h
        Source/Common/System/Crc32.h
        Source/Common/ObjectModel/Object.h
        Source/Common/ObjectModel/Object.cpp
        Source/Common/ObjectModel/Reflection/ClassMember.h
        Source/Common/Container/Array.h
        Source/Common/ObjectModel/Reflection/ObjectReader.h
        Source/Common/ObjectModel/Reflection/Reader/JsonReader.h
        Source/Common/ObjectModel/Reflection/Reader/JsonReader.cpp
        Source/Common/ObjectModel/Reflection/ClassEnum.h
        Source/Common/ObjectModel/Reflection/ClassEnum.h
        Source/Common/ObjectModel/Reflection/EnumValue.h
        Source/Common/ObjectModel/Reflection/EnumValue.h
        Source/Common/Container/FixedArray.h
        Source/Common/ObjectModel/Reflection/ObjectWriter.h
        Source/Common/ObjectModel/Reflection/Writer/JsonWriter.cpp
        Source/Common/ObjectModel/Reflection/Writer/JsonWriter.h
        Source/Common/ObjectModel/Reflection/ClassMethod.h
        Source/Common/Exception/ValueNotFound.h
)

find_package(nlohmann_json)
target_link_libraries(VigilSDK nlohmann_json::nlohmann_json)

add_subdirectory(UnitTest)