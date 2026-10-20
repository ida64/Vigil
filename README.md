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


Additionally, the access specifier used with fields is taken into account when generating the relevant class info, **non-public member variables will be ignored.**
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

public: // Reflected Members
    vgBool Foo;
    vgU32 Bar;

private: // Non-Reflected Members
    vgBool Baz;

}; // class TestObject

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
git clone https://github.com/ida64/VigilSDK.git
cd VigilSDK
./build.sh
```
And that should be it! This script builds both Release and Debug Lib/UnitTest.

## License

[MIT](https://choosealicense.com/licenses/mit/)


## C++23 component schemas

The additive `vigil::schema` CMake target provides header-only, plain-struct reflection
without `Object` inheritance, JSON, the Clang parser, or runtime registration. The
original SDK remains available unchanged through `VigilSDK`.

```cpp
#include <vigil/binary.hpp>

struct Position
{
public: // Methods
    Position(double x = 0, double y = 0) noexcept : m_X(x), m_Y(y)
    {
    }
    double GetX() const noexcept
    {
        return m_X;
    }
    double GetY() const noexcept
    {
        return m_Y;
    }
    void SetX(double value) noexcept
    {
        m_X = value;
    }
    void SetY(double value) noexcept
    {
        m_Y = value;
    }

private: // Friends
    friend struct vigil::Schema<Position>;

private: // Member Variables
    double m_X;
    double m_Y;
}; // struct Position

template <> struct vigil::Schema<Position>
{
public: // Static Variables
    static constexpr auto ms_Descriptor = vigil::Component<Position>(
        "game.Position", 1,
        vigil::Field<&Position::m_X>("x"),
        vigil::Field<&Position::m_Y>("y"));
}; // struct vigil::Schema<Position>

static_assert(vigil::ComponentId<Position> == vigil::Id("game.Position"));
```

`Component` generates typed member-pointer metadata and a static array of erased
`FieldDescriptor` values at compile time. `Describe<T>()` returns metadata by
reference; `FindField<T>(Id("x"))` searches numeric identifiers with no allocation
or string comparison. Cache the result outside simulation loops; simulation systems
should use component APIs that inline to ordinary member access. `Read` and `Write` provide
inspection and plugin binding access to valid component instances, including structs
with nonstandard layouts. They are a C++ SDK boundary, not a stable C ABI or plugin
loader. No ECS is coupled to reflection. Component fields stay private: grant the matching
`Schema<T>` specialization friendship to describe them. Descriptors expose read-only
`GetId`, `GetName`, `GetKind`, `GetSize`, `GetVersion`, `GetFields`, and `GetInspection`
accessors as appropriate; the generated metadata remains immutable. `BinaryLimits`
exposes `GetMaxBytes`/`SetMaxBytes` and corresponding methods for sequence elements,
string bytes, nesting depth, and aggregate decoded bytes.

Identifiers use stable FNV-1a over explicit names. Use globally qualified component
names and keep them stable across builds. Duplicate field identifiers are rejected
at compile time; application registries must reject component ID collisions. Names
passed to `Component` and `Field` must have static lifetime (normally string literals).

`Serialize(value, output)` and `Deserialize(bytes, target)` return a `BinaryResult`
with `GetError()`; neither replaces its destination on a reported failure. Allocation
exceptions propagate. Decode requires default construction and noexcept move
assignment. Supported values are integers up to 64 bits, bools, IEEE floats, enums,
strings, vectors, arrays, and nested reflected components. Raw pointers and arbitrary
object representations are deliberately unsupported; enum domain validation belongs
to the application.

The binary format uses explicit little-endian integers: `VGSC` magic, 64-bit component
ID, 32-bit schema version and field count, then each 64-bit field ID, 32-bit payload
length and encoded value. String/sequence lengths are 32-bit. Nested components have
their own envelopes. Decoders reject unknown/missing/duplicate fields, schema version
mismatches, invalid bools, truncation and trailing bytes. Change schema versions whenever
a field's type or meaning changes; migrations are explicit application code, not silent
reinterpretations. This is a component persistence primitive, not a complete world save
format or Minecraft Anvil/NBT compatibility implementation.

`BinaryLimits` bounds input/output bytes, string bytes, sequence lengths, nesting,
and aggregate decoded dynamic storage (`m_MaxDecodedBytes`, excluding allocator
bookkeeping and preexisting/default-constructed application objects). Untrusted data
is checked before dynamic storage is allocated. A failed decode never mutates the
caller's component.

Build and test the independent API with CMake, Ninja and GoogleTest:

```sh
cmake -S . -B build-schema -G Ninja -DVIGIL_BUILD_LEGACY=OFF -DVIGIL_BUILD_SCHEMA_TESTS=ON
cmake --build build-schema
ctest --test-dir build-schema --output-on-failure
```

GoogleTest is found locally or fetched at a pinned commit. Set
`VIGIL_BUILD_SCHEMA_TESTS=OFF` for dependency-free embedding, then link
`vigil::schema` after `add_subdirectory(path/to/Vigil)`.
