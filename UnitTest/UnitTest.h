/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#ifndef VIGILSDK_UNITTEST_H
#define VIGILSDK_UNITTEST_H

#include <nlohmann/json.hpp>

#include <memory>
#include <Common/Container/Array.h>

#include <Common/ObjectModel/Object.h>
#include <Common/ObjectModel/Reflection/Reader/JsonReader.h>

using namespace vigil;

class TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

    vgBool Foo;
    vgU32 Bar;
    char Baz[32];

};
// +Reflection(ClassName:TestObject)
const ClassMember kTestObjectClassMembers[] = {
{ VG_CRC32("Foo"),"Foo",VG_CRC32("bool"),"bool",offsetof(TestObject, Foo),sizeof(TestObject),ClassMember::Flags_None },
{ VG_CRC32("Bar"),"Bar",VG_CRC32("uint32_t"),"uint32_t",offsetof(TestObject, Bar),sizeof(TestObject),ClassMember::Flags_None },
{ VG_CRC32("Baz"),"Baz",VG_CRC32("char"),"char",offsetof(TestObject, Baz),sizeof(TestObject),ClassMember::Flags_ConstantArray },
};
VG_REFLECTED_IMPL(TestObject)
// -Reflection(ClassName:TestObject)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>


#endif //VIGILSDK_UNITTEST_H