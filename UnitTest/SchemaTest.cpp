#include <algorithm>
#include <gtest/gtest.h>
#include <vigil/binary.hpp>
struct Position
{
public: // Methods
    Position(double x = 0, std::int32_t y = 0, bool grounded = false) noexcept
        : m_X(x), m_Y(y), m_Grounded(grounded)
    {
    }
    double GetX() const noexcept
    {
        return m_X;
    }
    std::int32_t GetY() const noexcept
    {
        return m_Y;
    }
    bool GetGrounded() const noexcept
    {
        return m_Grounded;
    }
    void SetX(double value) noexcept
    {
        m_X = value;
    }
    void SetY(std::int32_t value) noexcept
    {
        m_Y = value;
    }
    void SetGrounded(bool value) noexcept
    {
        m_Grounded = value;
    }

private: // Friends
    friend struct vigil::Schema<Position>;

private: // Member Variables
    double m_X;
    std::int32_t m_Y;
    bool m_Grounded;
}; // struct Position
struct Actor
{
public: // Methods
    Actor(Position position = {}, std::string name = {}, std::vector<std::uint16_t> inventory = {},
          std::array<float, 2> velocity = {})
        : m_Position(position), m_Name(std::move(name)), m_Inventory(std::move(inventory)), m_Velocity(velocity)
    {
    }
    const Position &GetPosition() const noexcept
    {
        return m_Position;
    }
    const std::string &GetName() const noexcept
    {
        return m_Name;
    }
    const std::vector<std::uint16_t> &GetInventory() const noexcept
    {
        return m_Inventory;
    }
    const std::array<float, 2> &GetVelocity() const noexcept
    {
        return m_Velocity;
    }
    void SetPosition(Position value) noexcept
    {
        m_Position = value;
    }
    void SetName(std::string value) noexcept
    {
        m_Name = std::move(value);
    }
    void SetInventory(std::vector<std::uint16_t> value) noexcept
    {
        m_Inventory = std::move(value);
    }
    void SetVelocity(std::array<float, 2> value) noexcept
    {
        m_Velocity = value;
    }

private: // Friends
    friend struct vigil::Schema<Actor>;

private: // Member Variables
    Position m_Position;
    std::string m_Name;
    std::vector<std::uint16_t> m_Inventory;
    std::array<float, 2> m_Velocity;
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
template <class T>
concept HasPublicX = requires(T value) { value.m_X; };
template <class T>
concept HasPublicDescriptorId = requires(T value) { value.m_Id; };
static_assert(!HasPublicX<Position>);
static_assert(!HasPublicDescriptorId<vigil::FieldDescriptor>);
static_assert(!HasPublicDescriptorId<std::remove_cvref_t<decltype(vigil::Describe<Position>())>>);

static_assert(vigil::ComponentId<Position> == vigil::Id("test.Position"));
static_assert(vigil::FindField<Position>(vigil::Id("y"))->GetSize() == sizeof(std::int32_t));

TEST(Schema, TypedAndPluginInspection)
{
    Position position{1.5, -64, true};
    position.SetX(2.5);
    position.SetGrounded(false);
    EXPECT_EQ(position.GetX(), 2.5);
    EXPECT_FALSE(position.GetGrounded());
    auto *descriptor = vigil::FindField<Position>(vigil::Id("y"));
    ASSERT_NE(descriptor, nullptr);
    EXPECT_EQ(descriptor->GetKind(), vigil::FieldKind::SignedInteger);
    EXPECT_EQ(*static_cast<const std::int32_t *>(descriptor->Read(&position)), -64);
    *static_cast<std::int32_t *>(descriptor->Write(&position)) = 20;
    EXPECT_EQ(position.GetY(), 20);
    EXPECT_EQ(vigil::FindField<Position>(vigil::Id("missing")), nullptr);
}
TEST(Binary, RoundTripNestedCollections)
{
    Actor source{{3.25, -64, true}, std::string("a\0b", 3), {0, 65535, 16}, {2.5f, -0.5f}};
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(source, bytes));
    Actor decoded;
    ASSERT_TRUE(vigil::Deserialize(bytes, decoded));
    EXPECT_EQ(decoded.GetPosition().GetX(), source.GetPosition().GetX());
    EXPECT_EQ(decoded.GetPosition().GetY(), source.GetPosition().GetY());
    EXPECT_EQ(decoded.GetPosition().GetGrounded(), source.GetPosition().GetGrounded());
    EXPECT_EQ(decoded.GetName(), source.GetName());
    EXPECT_EQ(decoded.GetInventory(), source.GetInventory());
    EXPECT_EQ(decoded.GetVelocity(), source.GetVelocity());
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
        EXPECT_EQ(target.GetName(), "unchanged");
        EXPECT_EQ(target.GetPosition().GetX(), 99);
    }
}
TEST(Binary, RejectsWrongComponentVersionAndTrailingData)
{
    std::vector<std::byte> original;
    ASSERT_TRUE(vigil::Serialize(Position{}, original));
    Position target;
    auto bytes = original;
    bytes[4] ^= std::byte{1};
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::WrongComponent);
    bytes = original;
    bytes[12] = std::byte{2};
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::UnsupportedVersion);
    bytes = original;
    bytes.push_back(std::byte{0});
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::TrailingData);
}
TEST(Binary, RejectsDuplicateUnknownAndInvalidBoolean)
{
    std::vector<std::byte> original;
    ASSERT_TRUE(vigil::Serialize(Position{}, original));
    Position target;
    // First record: 20-byte header, 12-byte Field header, 8-byte double.
    auto bytes = original;
    std::copy_n(bytes.begin() + 20, 8, bytes.begin() + 40);
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::DuplicateField);
    bytes = original;
    bytes[20] ^= std::byte{1};
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::InvalidFormat);
    bytes = original;
    bytes.back() = std::byte{2};
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::InvalidFormat);
}
TEST(Binary, EnforcesLimitsWithoutChangingOutput)
{
    Actor actor{{}, "long string", {1, 2, 3}, {}};
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(actor, bytes));
    Actor target;
    vigil::BinaryLimits limits;
    limits.SetMaxBytes(bytes.size() - 1);
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).GetError(), vigil::BinaryError::LimitExceeded);
    limits = {};
    limits.SetMaxStringBytes(2);
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).GetError(), vigil::BinaryError::LimitExceeded);
    limits = {};
    limits.SetMaxSequenceElements(1);
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).GetError(), vigil::BinaryError::LimitExceeded);
    limits = {};
    limits.SetMaxDepth(0);
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).GetError(), vigil::BinaryError::LimitExceeded);
    auto output = bytes;
    limits = {};
    limits.SetMaxBytes(1);
    EXPECT_EQ(vigil::Serialize(actor, output, limits).GetError(), vigil::BinaryError::LimitExceeded);
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
    EXPECT_EQ(vigil::Deserialize(bytes, target).GetError(), vigil::BinaryError::LimitExceeded);
}

TEST(Binary, BoundsAggregateDecodedStorage)
{
    Actor source{{}, "test", {1, 2, 3}, {}};
    std::vector<std::byte> bytes;
    ASSERT_TRUE(vigil::Serialize(source, bytes));
    vigil::BinaryLimits limits;
    limits.SetMaxDecodedBytes(9);
    Actor target;
    EXPECT_EQ(vigil::Deserialize(bytes, target, limits).GetError(), vigil::BinaryError::LimitExceeded);
    EXPECT_TRUE(target.GetName().empty());
    limits.SetMaxDecodedBytes(10);
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
                EXPECT_EQ(target.GetName(), "preserved");
                EXPECT_EQ(target.GetPosition().GetX(), 99);
            }
        }
    }
}
