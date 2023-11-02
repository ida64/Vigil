/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#ifndef VIGILSDK_UNITTEST_H
#define VIGILSDK_UNITTEST_H

#include <memory>

#include <Common/ObjectModel/Object.h>

using namespace vigil;

/** +Reflected(TestObjectReflection.cpp) */
class TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

    int Test;
    char Test2;
    bool Test3;

};
// +Reflection("TestObject")
const ClassMember kTestObjectClassMembers[] = {
{ ComputeCrc32("Test", VG_ARRAY_SIZE("Test") - 1), "Test", ComputeCrc32("int", VG_ARRAY_SIZE("int") - 1), "int",0,sizeof(int) },
{ ComputeCrc32("Test2", VG_ARRAY_SIZE("Test2") - 1), "Test2", ComputeCrc32("char", VG_ARRAY_SIZE("char") - 1), "char",0,sizeof(char) },
{ ComputeCrc32("Test3", VG_ARRAY_SIZE("Test3") - 1), "Test3", ComputeCrc32("bool", VG_ARRAY_SIZE("bool") - 1), "bool",0,sizeof(bool) },
};
VG_REFLECTED_IMPL(TestObject)
// -Reflection("TestObject")

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>


#endif //VIGILSDK_UNITTEST_H