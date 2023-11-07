# Vigil

C++ Runtime Reflection, With Clang Back-End. 

![Logo](https://i.ibb.co/n7K7vP3/vigil-logo.png)

## Features
- Class Reflection
	- Constructor
	- Fields
	- Enumerations
	- Arrays
- JSON Serialization & Deserialization
- Easy To Use Marker Design

## Builds and Platform Support
- Microsoft Visual C++ (MSVC) on Windows  [✔️]
- GNU Compiler Collection (GCC) on Ubuntu [✔️]
- Clang on Ubuntu                         [✔️]

[![Multi-Platform Build](https://github.com/ZelSoftware/VigilSDK/actions/workflows/cmake-multi-platform.yml/badge.svg?branch=master)](https://github.com/ZelSoftware/VigilSDK/actions/workflows/cmake-multi-platform.yml)

**General Usage**, here are two samples from [UnitTest/UnitTest.cpp](https://github.com/ZelSoftware/Vigil/blob/master/UnitTest/UnitTest.cpp) that shows the `GetValueOf` and `SetValueOf` functions in action.
```cpp
auto* objectA = testObject->GetValueOf<TestObject2*>("A");  
CHECK_NE(objectA, nullptr);  
  
CHECK_EQ(objectA->GetValueOf<vgBool>("Foo"), true);  
CHECK_EQ(objectA->GetValueOf<vgFloat>("Bar"), 3.0);  
CHECK_EQ(objectA->GetValueOf<vgS32>("Baz"), 42);
```

```cpp
objectA->SetValueOf<vgBool>("Foo", false);  
objectA->SetValueOf<vgFloat>("Bar", 2.0);  
objectA->SetValueOf<vgS32>("Baz", 22);

CHECK_EQ(objectA->GetValueOf<vgBool>("Foo"), false);  
CHECK_EQ(objectA->GetValueOf<vgFloat>("Bar"), 2.0);  
CHECK_EQ(objectA->GetValueOf<vgS32>("Baz"), 22);
```

## Getting Started

**Class Reflection**, in this example it shows a sample Object with Foo and Bar member variables. 

**Note:** When you want to use class reflection, it's important to add the +/- Reflection markers and the `VG_CLASS_REFLECTION` attribute marker so that the code parser knows where to insert the class info.
***
```cpp
#include <Common/ObjectModel/Object.h>

class VG_CLASS_REFLECTION TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    // You must declare this virtual function in order to use reflection.
    virtual Class* GetClass() const override;

    vgBool Foo;
    vgU32 Bar;

};

// +Reflection(ClassName:TestObject)
// -Reflection(ClassName:TestObject)
```

Run the `Reflect.py` script.
```
usage: Vigil Class Generator [-h] [-file file [file ...]] [-v [cpp_version]] [-llvm_path [llvm_path]] [-save_tmp [save_tmp]]

Generates reflection code for classes

options:
  -h, --help            show this help message and exit
  -file file [file ...]
                        file to parse
  -v [cpp_version], --cpp_version [cpp_version]
                        C++ version to use
  -llvm_path [llvm_path]
                        Path to libclang
  -save_tmp [save_tmp]  Save temporary file after parsing
```

To add reflection to your class, you can run the following command:
```
py .\Reflect.py -file <file> -llvm_path <path to libclang>
```

And that's it! You should now be ready to use the reflection api and all it has to offer.

## Build
VigilSDK is compatible with both Windows and Linux. With **cmake** and **conan**, you can build the project with the following commands:
```bash
git clone https://github.com/ZelSoftware/VigilSDK.git
cd VigilSDK
./build.sh
```
And that should be it! This script builds both Release and Debug Lib/UnitTest.

## License

[MIT](https://choosealicense.com/licenses/mit/)

