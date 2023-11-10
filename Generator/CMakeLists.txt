cmake_minimum_required(VERSION 3.26)
project(VigilGenerator)

set(CMAKE_CXX_STANDARD 20)

find_package(argparse)
find_package(absl)
find_package(plog)
find_package(range-v3)

include_directories(Source)

include_directories("C:/Program Files/LLVM/include")
link_directories("C:/Program Files/LLVM/lib")

add_executable(VigilGenerator Source/Main.cpp
        Source/Main.h
        Source/Compiler/Compiler.h
        Source/Compiler/Specification/BaseSpec.h
        Source/Compiler/Compiler.cpp
        Source/Compiler/Builder/TypeBuilder.h
        Source/Compiler/ClangUtils.h
        Source/Compiler/ClangUtils.cpp
        Source/Compiler/Io/Log.h
        Source/Compiler/Io/Log.cpp
        Source/Compiler/Builder/ClassBuilder.h
        Source/Compiler/Builder/ClassBuilder.cpp
        Source/Compiler/Builder/FieldBuilder.cpp
        Source/Compiler/Builder/FieldBuilder.h
)

target_link_libraries(VigilGenerator libclang argparse::argparse abseil::abseil plog::plog range-v3::range-v3)
