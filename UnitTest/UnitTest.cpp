/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/

#include "UnitTest.h"

#ifdef GetClassName
#undef GetClassName
#endif

TEST_CASE("JsonReader")
{
    nlohmann::json testJson = R"(
        {
            "A": {
                "Foo": true,
                "Bar": 3.0,
                "Baz": 42
            },
            "B": {
                "Foo": false,
                "Bar": 2.0,
                "Baz": 22
            },
            "C": [
                {
                    "Foo": true,
                    "Bar": 2.0,
                    "Baz": 22
                },
                {
                    "Foo": false,
                    "Bar": 2.0,
                    "Baz": 22
                }
            ]
        }
    )"_json;

    const auto& testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    SUBCASE("Object Array")
    {
        CHECK_NE(testObject->C[0], nullptr);
        CHECK_NE(testObject->C[1], nullptr);

        CHECK_EQ(testObject->C[0]->Foo, true);
        CHECK_EQ(testObject->C[0]->Bar, 2.0);
        CHECK_EQ(testObject->C[0]->Baz, 22);

        CHECK_EQ(testObject->C[1]->Foo, false);
        CHECK_EQ(testObject->C[1]->Bar, 2.0);
        CHECK_EQ(testObject->C[1]->Baz, 22);
    }

    CHECK_NE(testObject->A, nullptr);

    SUBCASE("Read")
    {
        CHECK_EQ(testObject->A->Foo, true);
        CHECK_EQ(testObject->A->Bar, 3.0);
        CHECK_EQ(testObject->A->Baz, 42);

        CHECK_EQ(testObject->B->Foo, false);
        CHECK_EQ(testObject->B->Bar, 2.0);
        CHECK_EQ(testObject->B->Baz, 22);
    }

    SUBCASE("GetValueOf")
    {
        auto* objectA = testObject->GetValueOf<TestObject2*>("A");
        CHECK_NE(objectA, nullptr);

        CHECK_EQ(objectA->GetValueOf<vgBool>("Foo"), true);
        CHECK_EQ(objectA->GetValueOf<vgFloat>("Bar"), 3.0);
        CHECK_EQ(objectA->GetValueOf<vgS32>("Baz"), 42);
    }

    SUBCASE("SetValueOf")
    {
        auto* objectA = testObject->GetValueOf<TestObject2*>("A");
        CHECK_NE(objectA, nullptr);

        objectA->SetValueOf<vgBool>("Foo", false);
        objectA->SetValueOf<vgFloat>("Bar", 2.0);
        objectA->SetValueOf<vgS32>("Baz", 22);

        CHECK_EQ(objectA->GetValueOf<vgBool>("Foo"), false);
        CHECK_EQ(objectA->GetValueOf<vgFloat>("Bar"), 2.0);
        CHECK_EQ(objectA->GetValueOf<vgS32>("Baz"), 22);
    }
}

TEST_CASE("JsonWriter")
{
    nlohmann::json testJson = R"(
        {
            "A": {
                "Foo": true,
                "Bar": 3.0,
                "Baz": 42
            },
            "B": {
                "Foo": false,
                "Bar": 2.0,
                "Baz": 22
            },
            "C": [
                {
                    "Foo": true,
                    "Bar": 2.0,
                    "Baz": 22
                },
                {
                    "Foo": false,
                    "Bar": 2.0,
                    "Baz": 22
                }
            ]
        }
    )"_json;

    const auto& testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    JsonWriter testWriter;
    CHECK_EQ(Object::Serialize(testObject, testWriter), true);

    std::cout << testWriter.GetJson().dump() << std::endl;

    SUBCASE("Write")
    {
        CHECK_EQ(testWriter.GetJson(), testJson);
    }
}

TEST_CASE("Static Method")
{
    Class* objClass = GetClassByID(VG_CRC32("TestObject"));
    CHECK_NE(objClass, nullptr);

    CHECK_EQ(objClass->CallMethod<int>("Test2", 1, 2), 3);
}