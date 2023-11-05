conan install .

cmake -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install -B build -S .
cmake --build build --config Debug --target ALL_BUILD

# Build Release
cmake -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -B build -S .
cmake --build build --config Release --target ALL_BUILD
