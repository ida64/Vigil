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

class VG_CLASS_REFLECTION TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

    vgBool Foo{};
    vgU32 Bar{};
    vgFloat Baz{};

    vgS32 TestArray[5]{};

};
// +Reflection(ClassName:TestObject)
const vigil::ClassMember kTestObjectClassMembers[] = {
    { VG_CRC32("Foo"),"Foo",VG_CRC32("bool"),"bool",offsetof(TestObject, Foo),sizeof(bool),ClassMember::Flags_None },
    { VG_CRC32("Bar"),"Bar",VG_CRC32("uint32_t"),"uint32_t",offsetof(TestObject, Bar),sizeof(uint32_t),ClassMember::Flags_None },
    { VG_CRC32("Baz"),"Baz",VG_CRC32("float"),"float",offsetof(TestObject, Baz),sizeof(float),ClassMember::Flags_None },
    { VG_CRC32("TestArray"),"TestArray",VG_CRC32("int32_t"),"int32_t",offsetof(TestObject, TestArray),sizeof(int32_t[5]),ClassMember::Flags_ConstantArray },
};

const vigil::FixedArray<const ClassEnum*, 0> kTestObjectEnums = {  };
VG_REFLECTED_IMPL(TestObject);
// -Reflection(ClassName:TestObject)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>


#endif //VIGILSDK_UNITTEST_H