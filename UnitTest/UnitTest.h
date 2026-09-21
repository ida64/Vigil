/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#ifndef VIGILSDK_UNITTEST_H
#define VIGILSDK_UNITTEST_H

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
    { VG_CRC32("Foo"),"Foo",VG_CRC32("bool"),"bool",offsetof(TestObject2, Foo),sizeof(bool),ClassMember::Flags_None },
    { VG_CRC32("Bar"),"Bar",VG_CRC32("float"),"float",offsetof(TestObject2, Bar),sizeof(float),ClassMember::Flags_None },
    { VG_CRC32("Baz"),"Baz",VG_CRC32("int32_t"),"int32_t",offsetof(TestObject2, Baz),sizeof(int32_t),ClassMember::Flags_None },
};

const vigil::FixedArray<const ClassEnum*, 0> kTestObject2Enums = {  };
VG_REFLECTED_IMPL(TestObject2);
// -Reflection(ClassName:TestObject2)

class VG_CLASS_REFLECTION TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

    TestObject2* A;
    TestObject2* B;

};
// +Reflection(ClassName:TestObject)
const vigil::ClassMember kTestObjectClassMembers[] = {
    { VG_CRC32("A"),"A",VG_CRC32("TestObject2"),"TestObject2",offsetof(TestObject, A),sizeof(TestObject2 *),ClassMember::Flags_Pointer },
    { VG_CRC32("B"),"B",VG_CRC32("TestObject2"),"TestObject2",offsetof(TestObject, B),sizeof(TestObject2 *),ClassMember::Flags_Pointer },
};

const vigil::FixedArray<const ClassEnum*, 0> kTestObjectEnums = {  };
VG_REFLECTED_IMPL(TestObject);
// -Reflection(ClassName:TestObject)

class VG_CLASS_REFLECTION TestArrayObject : public Object
{
public:
    virtual Class* GetClass() const override;

    vgU32 Values[4] = { 9, 9, 9, 9 };
    char Name[5] = { 'x', 'x', 'x', 'x', '\0' };
    vgS32 Signed32 = 0;
    vgS64 Signed64 = 0;
};

const vigil::ClassMember kTestArrayObjectClassMembers[] = {
    { VG_CRC32("Values"), "Values", VG_CRC32("uint32_t"), "uint32_t", offsetof(TestArrayObject, Values), sizeof(TestArrayObject::Values), ClassMember::Flags_ConstantArray },
    { VG_CRC32("Name"), "Name", VG_CRC32("char"), "char", offsetof(TestArrayObject, Name), sizeof(TestArrayObject::Name), ClassMember::Flags_ConstantArray },
    { VG_CRC32("Signed32"), "Signed32", VG_CRC32("int32_t"), "int32_t", offsetof(TestArrayObject, Signed32), sizeof(vgS32), ClassMember::Flags_None },
    { VG_CRC32("Signed64"), "Signed64", VG_CRC32("int64_t"), "int64_t", offsetof(TestArrayObject, Signed64), sizeof(vgS64), ClassMember::Flags_None },
};

const vigil::FixedArray<const ClassEnum*, 0> kTestArrayObjectEnums = {  };
VG_REFLECTED_IMPL(TestArrayObject);

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#endif //VIGILSDK_UNITTEST_H
