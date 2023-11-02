/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_UNITTEST_H
#define VIGILSDK_UNITTEST_H

#include <memory>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <Common/ObjectModel/Object.h>

using namespace vigil;

class TestObject : public Object
{
public: // Constructors and Destructor
    TestObject() = default;
    virtual ~TestObject() = default;

public: // Methods
    virtual Class* GetClass() const override;

private: // Member Variables
    int m_Test;
public:
    VG_CLASS_MEMBER(int, Test);

};

VG_CLASS_MEMBER_ARRAY(TestObject)
{
    &TestObject::ms_TestClassMember
};

VG_REFLECTED_IMPL(TestObject);

#endif //VIGILSDK_UNITTEST_H
