/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#ifndef VIGILSDK_UNITTEST_H
#define VIGILSDK_UNITTEST_H

typedef signed char vgChar;
typedef unsigned char vgUChar;
typedef unsigned char vgByte;
typedef unsigned char vgU8;
typedef signed char vgS8;
typedef signed short vgShort;
typedef unsigned short vgUShort;
typedef signed int vgS32;
typedef unsigned int vgU32;
typedef signed long long vgS64;
typedef unsigned long long vgU64;
typedef float vgFloat;
typedef double vgDouble;
typedef bool vgBool;
typedef const char* vgString;

#define VG_CLASS_REFLECTION __attribute__((annotate("VG_CLASS_REFLECTION")))

#include <iostream>
#include <memory>

#include <Common/ObjectModel/Object.h>
#include <Common/Container/Array.h>
#include <Common/Container/FixedArray.h>

#include <Common/ObjectModel/Reflection/Reader/JsonReader.h>
#include <Common/ObjectModel/Reflection/Writer/JsonWriter.h>

using namespace vigil;

class VG_CLASS_REFLECTION TestObject2 : public Object
{
public: // Constructors and Destructor
    TestObject2() = default;
    virtual ~TestObject2() = default;

public: // Methods
    virtual Class* GetClass() const override;

    vgBool Foo;
    vgFloat Bar;
    vgS32 Baz;

};

// +Reflection(ClassName:TestObject2)
const vigil::ClassMember kTestObject2ClassMembers[] = {
{ VG_CRC32("Foo"),"Foo",VG_CRC32("bool"),"bool",offsetof(TestObject2, Foo),sizeof(bool), },
{ VG_CRC32("Bar"),"Bar",VG_CRC32("float"),"float",offsetof(TestObject2, Bar),sizeof(float), },
{ VG_CRC32("Baz"),"Baz",VG_CRC32("int32_t"),"int32_t",offsetof(TestObject2, Baz),sizeof(int32_t), },
};

const vigil::ClassMethod kTestObject2StaticMethods[] = {
};
const vigil::FixedArray<const ClassEnum*, 0> kTestObject2Enums = {  };
VG_REFLECTED_IMPL(TestObject2, VG_CRC32("Object"));
// -Reflection(ClassName:TestObject2)

class VG_CLASS_REFLECTION BaseTestObject : public Object
{
public: // Constructors and Destructor
    BaseTestObject() = default;

    virtual ~BaseTestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;
};

// +Reflection(ClassName:BaseTestObject)
const vigil::ClassMember kBaseTestObjectClassMembers[] = {
};

const vigil::ClassMethod kBaseTestObjectStaticMethods[] = {
};
const vigil::FixedArray<const ClassEnum*, 0> kBaseTestObjectEnums = {  };
VG_REFLECTED_IMPL(BaseTestObject, VG_CRC32("Object"));
// -Reflection(ClassName:BaseTestObject)

class VG_CLASS_REFLECTION TestObject : public BaseTestObject
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

    VG_SAFE_FIELD std::shared_ptr<TestObject2> A;
    VG_SAFE_FIELD std::shared_ptr<TestObject2> B;

    VG_SAFE_FIELD std::shared_ptr<TestObject2> C[2];

    static void TestReflectFn()
    {
        std::cout << "Hello from TestReflectFn!" << std::endl;
    }

    static void TestReflectFn2()
    {
        std::cout << "Hello from TestReflectFn2!" << std::endl;
    }

    static int Test2(int x, int y)
    {
        return x + y;
    }

};

// +Reflection(ClassName:TestObject)
const vigil::ClassMember kTestObjectClassMembers[] = {
{ VG_CRC32("A"),"A",VG_CRC32("TestObject2"),"TestObject2",offsetof(TestObject, A),sizeof(std::shared_ptr<TestObject2>),vigil::ClassMember::Flags_Pointer | vigil::ClassMember::Flags_SafeField },
{ VG_CRC32("B"),"B",VG_CRC32("TestObject2"),"TestObject2",offsetof(TestObject, B),sizeof(std::shared_ptr<TestObject2>),vigil::ClassMember::Flags_Pointer | vigil::ClassMember::Flags_SafeField },
{ VG_CRC32("C"),"C",VG_CRC32("TestObject2"),"TestObject2",offsetof(TestObject, C),sizeof(std::shared_ptr<TestObject2>[2]),vigil::ClassMember::Flags_ConstantArray | vigil::ClassMember::Flags_Pointer | vigil::ClassMember::Flags_SafeField },
};

const vigil::ClassMethod kTestObjectStaticMethods[] = {
{ "TestReflectFn",VG_CRC32("TestReflectFn"),(void*)TestObject::TestReflectFn,MethodType_Static },
{ "TestReflectFn2",VG_CRC32("TestReflectFn2"),(void*)TestObject::TestReflectFn2,MethodType_Static },
{ "Test2",VG_CRC32("Test2"),(void*)TestObject::Test2,MethodType_Static },
};
const vigil::FixedArray<const ClassEnum*, 0> kTestObjectEnums = {  };
VG_REFLECTED_IMPL(TestObject, VG_CRC32("BaseTestObject"));
// -Reflection(ClassName:TestObject)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#endif //VIGILSDK_UNITTEST_H