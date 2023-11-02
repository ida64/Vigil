/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/

#include "UnitTest.h"

#ifdef GetClassName
#undef GetClassName
#endif

TEST_CASE("Object")
{
    std::shared_ptr<TestObject> testObject = std::make_shared<TestObject>();

    SUBCASE("GetClass")
    {
        Class* testClass = testObject->GetClass();
        CHECK(testClass != nullptr);
    }

    SUBCASE("GetClassName")
    {
        Class* testClass = testObject->GetClass();
        CHECK(testClass->GetClassName() == "TestObject");
    }

    SUBCASE("GetClassID")
    {
        Class* testClass = testObject->GetClass();
        CHECK(testClass->GetClassID() == ComputeCrc32("TestObject", VG_ARRAY_SIZE("TestObject") - 1));
    }

    SUBCASE("GetNbMembers")
    {
        Class* testClass = testObject->GetClass();
        CHECK(testClass->GetNbMembers() == 3);
    }

    SUBCASE("GetMember")
    {
        Class* testClass = testObject->GetClass();
        ClassMember testMember = testClass->GetMember(0);
    }
}