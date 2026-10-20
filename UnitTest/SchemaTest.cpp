#include <algorithm>
#include <gtest/gtest.h>
#include <vigil/binary.hpp>
struct Position
{
public: // Member Variables
    double m_X = 0;
    std::int32_t m_Y = 0;
    bool m_Grounded = false;
}; // struct Position
struct Actor
{
public: // Member Variables
    Position m_Position;
    std::string m_Name;
    std::vector<std::uint16_t> m_Inventory;
    std::array<float, 2> m_Velocity{};
}; // struct Actor
struct Empty
{
};
template <> struct vigil::Schema<Position>
{
public: // Methods
    static constexpr auto ms_Descriptor =
        vigil::Component<Position>("test.Position", 1, vigil::Field<&Position::m_X>("x"),
                                   vigil::Field<&Position::m_Y>("y"), vigil::Field<&Position::m_Grounded>("grounded"));
};
template <> struct vigil::Schema<Actor>
{
public: // Methods
    static constexpr auto ms_Descriptor = vigil::Component<Actor>(
        "test.Actor", 2, vigil::Field<&Actor::m_Position>("position"), vigil::Field<&Actor::m_Name>("name"),
        vigil::Field<&Actor::m_Inventory>("inventory"), vigil::Field<&Actor::m_Velocity>("velocity"));
};
template <> struct vigil::Schema<Empty>
{
public: // Static Variables
    static constexpr auto ms_Descriptor = vigil::Component<Empty>("test.Empty", 1);
};
static_assert(vigil::ComponentId<Position> == vigil::Id("test.Position"));
static_assert(vigil::FindField<Position>(vigil::Id("y"))->m_Size == sizeof(std::int32_t));

TEST(Schema, TypedAndPluginInspection)
{
    Position position{1.5, -64, true};
    auto *descriptor = vigil::FindField<Position>(vigil::Id("y"));
    ASSERT_NE(descriptor, nullptr);
    EXPECT_EQ(descriptor->m_Kind, vigil::FieldKind::SignedInteger);
    EXPECT_EQ(*static_cast<const std::int32_t *>(descriptor->m_Read(&position)), -64);
    *static_cast<std::int32_t *>(descriptor->m_Write(&position)) = 20;
    EXPECT_EQ(position.m_Y, 20);
    EXPECT_EQ(vigil::FindField<Position>(vigil::Id("missing")), nullptr);
}
TEST(Binary, RoundTripNestedCollections)
{
    Actor source{{3.25, -64, true}, std::string("a\0b", 3), {0, 65535, 16}, {2.5f, -0.5f}};
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(source, bytes));
    Actor decoded;
    ASSERT_TRUE(vigil::Deserialize(bytes, decoded));
    EXPECT_EQ(decoded.m_Position.m_X, source.m_Position.m_X);
    EXPECT_EQ(decoded.m_Position.m_Y, source.m_Position.m_Y);
    EXPECT_EQ(decoded.m_Position.m_Grounded, source.m_Position.m_Grounded);
    EXPECT_EQ(decoded.m_Name, source.m_Name);
    EXPECT_EQ(decoded.m_Inventory, source.m_Inventory);
    EXPECT_EQ(decoded.m_Velocity, source.m_Velocity);
}
TEST(Binary, StableLittleEndianEnvelope)
{
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(Empty{}, bytes));
    ASSERT_EQ(bytes.size(), 20);
    EXPECT_EQ(bytes[0], std::byte{'V'});
    EXPECT_EQ(bytes[1], std::byte{'G'});
    EXPECT_EQ(bytes[2], std::byte{'S'});
    EXPECT_EQ(bytes[3], std::byte{'C'});
    EXPECT_EQ(bytes[12], std::byte{1});
    EXPECT_EQ(bytes[13], std::byte{0});
    for (std::size_t i = 0; i < 8; ++i)
        EXPECT_EQ(bytes[4 + i], std::byte((vigil::Id("test.Empty") >> (i * 8)) & 255));
}
TEST(Binary, AllTruncationsPreserveDestination)
{
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(Actor{{1, -4, true}, "test", {1, 2}, {}}, bytes));
    for (std::size_t length = 0; length < bytes.size(); ++length)
    {
        Actor target{{99, 42, false}, "unchanged", {7}, {}};
        EXPECT_FALSE(vigil::Deserialize(std::span(bytes).first(length), target)) << length;
        EXPECT_EQ(target.m_Name, "unchanged");
        EXPECT_EQ(target.m_Position.m_X, 99);
    }
}
TEST(Binary, RejectsWrongComponentVersionAndTrailingData)
{
    std::vector<std::byte> original;
    ASSERT_TRUE(vigil::Serialize(Position{}, original));
    Position target;
    auto bytes = original;
    bytes[4] ^= std::byte{1};
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::WrongComponent);
    bytes = original;
    bytes[12] = std::byte{2};
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::UnsupportedVersion);
    bytes = original;
    bytes.push_back(std::byte{0});
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::TrailingData);
}
TEST(Binary, RejectsDuplicateUnknownAndInvalidBoolean)
{
    std::vector<std::byte> original;
    ASSERT_TRUE(vigil::Serialize(Position{}, original));
    Position target;
    // First record: 20-byte header, 12-byte Field header, 8-byte double.
    auto bytes = original;
    std::copy_n(bytes.begin() + 20, 8, bytes.begin() + 40);
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::DuplicateField);
    bytes = original;
    bytes[20] ^= std::byte{1};
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::InvalidFormat);
    bytes = original;
    bytes.back() = std::byte{2};
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::InvalidFormat);
}
TEST(Binary, EnforcesLimitsWithoutChangingOutput)
{
    Actor actor{{}, "long string", {1, 2, 3}, {}};
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(actor, bytes));
    Actor target;
    vigil::BinaryLimits limits;
    limits.m_MaxBytes = bytes.size() - 1;
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).m_Error, vigil::BinaryError::LimitExceeded);
    limits = {};
    limits.m_MaxStringBytes = 2;
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).m_Error, vigil::BinaryError::LimitExceeded);
    limits = {};
    limits.m_MaxSequenceElements = 1;
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).m_Error, vigil::BinaryError::LimitExceeded);
    limits = {};
    limits.m_MaxDepth = 0;
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).m_Error, vigil::BinaryError::LimitExceeded);
    auto output = bytes;
    limits = {};
    limits.m_MaxBytes = 1;
    EXPECT_EQ(vigil::Serialize(actor, output, limits).m_Error, vigil::BinaryError::LimitExceeded);
    EXPECT_EQ(output, bytes);
}
TEST(Binary, HugeMalformedStringDoesNotAllocate)
{
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(Actor{}, bytes));
    // Locate the m_Name record by ms_Descriptor ID rather than relying on nested m_Size.
    auto fieldId = vigil::Id("name");
    std::size_t offset = 20;
    while (offset < bytes.size())
    {
        std::uint64_t Id = 0;
        std::uint32_t length = 0;
        for (std::size_t i = 0; i < 8; ++i)
            Id |= std::uint64_t(std::to_integer<unsigned char>(bytes[offset + i])) << (8 * i);
        for (std::size_t i = 0; i < 4; ++i)
            length |= std::uint32_t(std::to_integer<unsigned char>(bytes[offset + 8 + i])) << (8 * i);
        if (Id == fieldId)
        {
            for (int i = 0; i < 4; ++i)
                bytes[offset + 12 + i] = std::byte{255};
            break;
        }
        offset += 12 + length;
    }
    Actor target;
    EXPECT_EQ(vigil::Deserialize(bytes, target).m_Error, vigil::BinaryError::LimitExceeded);
}

TEST(Binary, BoundsAggregateDecodedStorage)
{
    Actor source{{}, "test", {1, 2, 3}, {}};
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(source, bytes));
    vigil::BinaryLimits limits;
    limits.m_MaxDecodedBytes = 9;
    Actor target;
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).m_Error, vigil::BinaryError::LimitExceeded);
    EXPECT_TRUE(target.m_Name.empty());
    limits.m_MaxDecodedBytes = 10;
    EXPECT_TRUE(vigil::Deserialize(bytes, target, limits));
}

TEST(Binary, MalformedMutationsRemainTransactional)
{
    const Actor source{{1.5, -64, true}, "hello", {0, 5}, {1, 2}};
    std::vector<std::byte> original;
    ASSERT_TRUE(vigil::Serialize(source, original));
    for (std::size_t offset = 0; offset < original.size(); ++offset)
    {
        for (const auto byte : {std::byte{0}, std::byte{255}})
        {
            auto bytes = original;
            bytes[offset] = byte;
            Actor target{{99, 42, false}, "preserved", {}, {}};
            const auto result = vigil::Deserialize(bytes, target);
            if (!result)
            {
                EXPECT_EQ(target.m_Name, "preserved");
                EXPECT_EQ(target.m_Position.m_X, 99);
            }
        }
    }
}
