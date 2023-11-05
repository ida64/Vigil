# Check if conan is installed
if ! command -v conan &> /dev/null
then
    echo "Conan could not be found"
    exit
fi

echo "Found conan, installing dependencies..."

# Install dependencies
conan install .

echo "Building debug..."

# Build Debug
cmake -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=install -B build -S .
cmake --build build --config Debug --target ALL_BUILD

echo "Building release..."

# Build Release
cmake -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -B build -S .
cmake --build build --config Release --target ALL_BUILD
