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
    const auto& testObject = std::make_shared<TestObject>();

    Class* testClass = testObject->GetClass();

    SUBCASE("GetClass")
    {
        CHECK_NE(testClass, nullptr);
    }

    SUBCASE("GetClassName")
    {
        CHECK_EQ(testClass->GetClassName(), "TestObject");
    }

    SUBCASE("GetClassID")
    {
        CHECK_EQ(testClass->GetClassID(), VG_CRC32("TestObject"));
    }

    SUBCASE("GetMember")
    {
        const ClassMember* testMember = testClass->GetMembers()[0];
        CHECK_EQ(testMember->GetName(), "Foo");
    }

}

TEST_CASE("JsonReader")
{
    nlohmann::json testJson = R"(
    {
        "Foo": true,
        "Bar": 2,
        "Baz": 3.14159
    }
    )"_json;

    const auto& testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    SUBCASE("Read")
    {
        CHECK_EQ(testObject->Foo, true);
        CHECK_EQ(testObject->Bar, 2);
        CHECK_EQ(testObject->Baz, 3.14159f);
    }
}

TEST_CASE("JsonWriter")
{
    nlohmann::json testJson = R"(
    {
        "Foo": true,
        "Bar": 2,
        "Baz": 3.14159
    }
    )"_json;

    const auto& testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    testObject->TestArray[0] = 1;
    testObject->TestArray[1] = 2;
    testObject->TestArray[2] = 3;
    testObject->TestArray[3] = 4;
    testObject->TestArray[4] = 5;

    JsonWriter testWriter;
    CHECK_EQ(Object::Serialize(testObject, testWriter), true);

    std::cout << testWriter.GetJson().dump(4) << std::endl;

    SUBCASE("Write")
    {
        CHECK_EQ(testWriter.GetJson()["Foo"], true);
        CHECK_EQ(testWriter.GetJson()["Bar"], 2);
        CHECK_EQ(testWriter.GetJson()["Baz"], 3.14159f);
    }
}

TEST_CASE("Array")
{
    SUBCASE("Resize")
    {
        Array<int> testArray;
        testArray.Resize(10);
        CHECK_EQ(testArray.GetSize(), 10);
    }

    SUBCASE("InitializerList")
    {
        Array<int> testArray2 = { 1, 2, 3, 4, 5 };
        CHECK_EQ(testArray2.GetSize(), 5);
    }

    SUBCASE("PushBack")
    {
        Array<int> testArray;
        testArray.PushBack(1);
        testArray.PushBack(2);
        testArray.PushBack(3);
        testArray.PushBack(4);
        testArray.PushBack(5);
        CHECK_EQ(testArray.GetSize(), 5);
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
        CHECK_EQ(testArray.GetSize(), 4);
    }

    SUBCASE("Clear")
    {
        Array<int> testArray;
        testArray.PushBack(1);
        testArray.PushBack(2);
        testArray.PushBack(3);

        testArray.Clear();

        CHECK_EQ(testArray.GetSize(), 0);
    }
}