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
            }
        }
    )"_json;

    const auto& testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    SUBCASE("Read")
    {
        CHECK_EQ(testObject->A->Foo, true);
        CHECK_EQ(testObject->A->Bar, 3.0);
        CHECK_EQ(testObject->A->Baz, 42);

        CHECK_EQ(testObject->B->Foo, false);
        CHECK_EQ(testObject->B->Bar, 2.0);
        CHECK_EQ(testObject->B->Baz, 22);
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
            }
        }
    )"_json;

    const auto& testObject = std::make_shared<TestObject>();

    JsonReader testReader(testJson);
    CHECK_EQ(Object::Deserialize(testObject, testReader), true);

    JsonWriter testWriter;
    CHECK_EQ(Object::Serialize(testObject, testWriter), true);

    SUBCASE("Write")
    {
        CHECK_EQ(testWriter.GetJson(), testJson);
    }
}
