cmake_minimum_required(VERSION 3.26)
project(VigilSDK)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(VigilSchema INTERFACE)
add_library(vigil::schema ALIAS VigilSchema)
target_compile_features(VigilSchema INTERFACE cxx_std_23)
target_include_directories(VigilSchema INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>)

option(VIGIL_BUILD_LEGACY "Build the original Object/JSON reflection library" ON)
option(VIGIL_BUILD_SCHEMA_TESTS "Build the plain-struct schema GoogleTest suite" OFF)
if(VIGIL_BUILD_SCHEMA_TESTS)
    include(CTest)
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723
            GIT_SHALLOW FALSE)
        FetchContent_MakeAvailable(googletest)
    endif()
    add_executable(VigilSchemaTest UnitTest/SchemaTest.cpp)
    target_link_libraries(VigilSchemaTest PRIVATE vigil::schema GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(VigilSchemaTest)
endif()

if(VIGIL_BUILD_LEGACY)
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
            Source/Common/ObjectModel/Reflection/Class.cpp
            Source/Common/Container/SimplePair.h
    )

    find_package(nlohmann_json)
    target_link_libraries(VigilSDK nlohmann_json::nlohmann_json)

    add_subdirectory(UnitTest)
endif()
