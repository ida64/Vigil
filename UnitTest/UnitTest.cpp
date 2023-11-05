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
        CHECK(testClass->GetNbMembers() == 2);
    }

    SUBCASE("GetMember")
    {
        Class* testClass = testObject->GetClass();
        ClassMember testMember = testClass->GetMember(0);
    }
}

TEST_CASE("JsonReader")
{
    nlohmann::json testJson = nlohmann::json::parse("{\"Foo\": true, \"Bar\": 2}");

    std::shared_ptr<TestObject> testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    SUBCASE("Read")
    {
        Class* testClass = testObject->GetClass();

        CHECK_EQ(testObject->Foo, true);
    }

}

TEST_CASE("Array")
{
    SUBCASE("Resize")
    {
        Array<int> testArray;
        testArray.Resize(10);
        CHECK(testArray.GetSize() == 10);
    }

    SUBCASE("InitializerList")
    {
        Array<int> testArray2 = { 1, 2, 3, 4, 5 };
        CHECK(testArray2.GetSize() == 5);
    }

    SUBCASE("PushBack")
    {
        Array<int> testArray;
        testArray.PushBack(1);
        testArray.PushBack(2);
        testArray.PushBack(3);
        testArray.PushBack(4);
        testArray.PushBack(5);
        CHECK(testArray.GetSize() == 5);
    }

    SUBCASE("PopBack")
    {
        Array<int> testArray;
        testArray.PushBack(1);
        testArray.PushBack(2);
        testArray.PushBack(3);
        testArray.PushBack(4);
        testArray.PushBack(5);
        testArray.PopBack();
        CHECK(testArray.GetSize() == 4);
    }

    SUBCASE("Clear")
    {
        Array<int> testArray;
        testArray.PushBack(1);
        testArray.PushBack(2);
        testArray.PushBack(3);

        testArray.Clear();

        CHECK(testArray.GetSize() == 0);
    }
}