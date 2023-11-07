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
// -Reflection(ClassName:TestObject)

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#endif //VIGILSDK_UNITTEST_H