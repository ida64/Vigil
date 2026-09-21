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

TEST_CASE("JsonReader safely handles fixed arrays")
{
    const auto& testObject = std::make_shared<TestArrayObject>();
    const auto members = testObject->GetClass()->GetMembers();

    SUBCASE("reads an exact-size numeric array")
    {
        const auto json = R"({"Values":[1,2,3,4]})"_json;
        JsonReader reader(json);
        CHECK(reader.Read(testObject.get(), members[0]));
        CHECK_EQ(testObject->Values[0], 1);
        CHECK_EQ(testObject->Values[3], 4);
    }

    SUBCASE("zero-fills an undersized numeric array")
    {
        const auto json = R"({"Values":[1,2]})"_json;
        JsonReader reader(json);
        CHECK(reader.Read(testObject.get(), members[0]));
        CHECK_EQ(testObject->Values[0], 1);
        CHECK_EQ(testObject->Values[1], 2);
        CHECK_EQ(testObject->Values[2], 0);
        CHECK_EQ(testObject->Values[3], 0);
    }

    SUBCASE("zero-fills an empty numeric array")
    {
        const auto json = R"({"Values":[]})"_json;
        JsonReader reader(json);
        CHECK(reader.Read(testObject.get(), members[0]));
        CHECK_EQ(testObject->Values[0], 0);
        CHECK_EQ(testObject->Values[3], 0);
    }

    SUBCASE("rejects an oversized numeric array without modifying the destination")
    {
        const auto json = R"({"Values":[1,2,3,4,5]})"_json;
        JsonReader reader(json);
        CHECK_FALSE(reader.Read(testObject.get(), members[0]));
        CHECK_EQ(testObject->Values[0], 9);
        CHECK_EQ(testObject->Values[3], 9);
    }

    SUBCASE("reads and terminates a character array")
    {
        const auto json = R"({"Name":"Vigi"})"_json;
        JsonReader reader(json);
        CHECK(reader.Read(testObject.get(), members[1]));
        CHECK_EQ(std::string(testObject->Name), "Vigi");
        CHECK_EQ(testObject->Name[4], '\0');
    }

    SUBCASE("rejects a string with no room for a terminator")
    {
        const auto json = R"({"Name":"Vigil"})"_json;
        JsonReader reader(json);
        CHECK_FALSE(reader.Read(testObject.get(), members[1]));
        CHECK_EQ(std::string(testObject->Name), "xxxx");
    }
}

TEST_CASE("JsonWriter bounds character arrays and preserves signed values")
{
    const auto& testObject = std::make_shared<TestArrayObject>();
    testObject->Name[0] = 'V';
    testObject->Name[1] = 'i';
    testObject->Name[2] = 'g';
    testObject->Name[3] = 'i';
    testObject->Name[4] = 'l';
    testObject->Signed32 = -42;
    testObject->Signed64 = -9223372036854775807LL;

    JsonWriter writer;
    CHECK(Object::Serialize(testObject, writer));
    CHECK_EQ(writer.GetJson()["Name"], "Vigil");
    CHECK_EQ(writer.GetJson()["Signed32"], -42);
    CHECK_EQ(writer.GetJson()["Signed64"], -9223372036854775807LL);

    const auto& roundTripObject = std::make_shared<TestArrayObject>();
    JsonReader reader(writer.GetJson());
    CHECK(Object::Deserialize(roundTripObject, reader));
    CHECK_EQ(roundTripObject->Signed32, -42);
    CHECK_EQ(roundTripObject->Signed64, -9223372036854775807LL);
}
