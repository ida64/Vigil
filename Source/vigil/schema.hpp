#ifndef VIGIL_SCHEMA_HPP
#define VIGIL_SCHEMA_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace vigil
{
using Identifier = std::uint64_t;

/** @brief Stable FNV-1a identifier over an explicit UTF-8 name. */
constexpr Identifier Id(std::string_view name) noexcept
{
    Identifier value = 14695981039346656037ull;
    for (unsigned char c : name)
    {
        value ^= c;
        value *= 1099511628211ull;
    }
    return value;
}

template <class T> struct Schema;
template <class T>
concept Reflected = requires { Schema<T>::ms_Descriptor; };

enum class FieldKind
{
    Boolean,
    SignedInteger,
    UnsignedInteger,
    FloatingPoint,
    Enumeration,
    Component,
    Sequence,
    Opaque
}; // enum class FieldKind

template <class T> consteval FieldKind KindOf()
{
    if constexpr (std::same_as<T, bool>)
        return FieldKind::Boolean;
    else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
        return FieldKind::SignedInteger;
    else if constexpr (std::is_integral_v<T>)
        return FieldKind::UnsignedInteger;
    else if constexpr (std::is_floating_point_v<T>)
        return FieldKind::FloatingPoint;
    else if constexpr (std::is_enum_v<T>)
        return FieldKind::Enumeration;
    else if constexpr (Reflected<T>)
        return FieldKind::Component;
    else if constexpr (requires(T t) { t.begin(); t.end(); })
        return FieldKind::Sequence;
    else
        return FieldKind::Opaque;
}

/** @brief Immutable metadata; accessors require an instance of the owning component. */
struct FieldDescriptor
{
public: // Types
    using ReadAccessor = const void *(*)(const void *) noexcept;
    using WriteAccessor = void *(*)(void *) noexcept;

public: // Methods
    constexpr FieldDescriptor(Identifier id, std::string_view name, FieldKind kind, std::size_t size,
                              ReadAccessor read, WriteAccessor write) noexcept
        : m_Id(id), m_Name(name), m_Kind(kind), m_Size(size), m_Read(read), m_Write(write)
    {
    }
    constexpr Identifier GetId() const noexcept
    {
        return m_Id;
    }
    constexpr std::string_view GetName() const noexcept
    {
        return m_Name;
    }
    constexpr FieldKind GetKind() const noexcept
    {
        return m_Kind;
    }
    constexpr std::size_t GetSize() const noexcept
    {
        return m_Size;
    }
    const void *Read(const void *object) const noexcept
    {
        return m_Read(object);
    }
    void *Write(void *object) const noexcept
    {
        return m_Write(object);
    }

private: // Member Variables
    Identifier m_Id;
    std::string_view m_Name;
    FieldKind m_Kind;
    std::size_t m_Size;
    ReadAccessor m_Read;
    WriteAccessor m_Write;
}; // struct FieldDescriptor

template <class> struct MemberTraits;
template <class T, class Owner> struct MemberTraits<T Owner::*>
{
    using ValueType = T;
    using OwnerType = Owner;
}; // struct MemberTraits

template <auto Member> struct MemberField
{
public: // Types
    using ValueType = typename MemberTraits<decltype(Member)>::ValueType;
    using OwnerType = typename MemberTraits<decltype(Member)>::OwnerType;

public: // Static Variables
    static constexpr auto ms_Pointer = Member;

public: // Methods
    constexpr MemberField(std::string_view name, Identifier id) noexcept : m_Name(name), m_Id(id)
    {
    }
    constexpr std::string_view GetName() const noexcept
    {
        return m_Name;
    }
    constexpr Identifier GetId() const noexcept
    {
        return m_Id;
    }
    constexpr FieldDescriptor Erase() const noexcept
    {
        return {m_Id, m_Name, KindOf<ValueType>(), sizeof(ValueType),
                [](const void *object) noexcept -> const void *
                { return &(static_cast<const OwnerType *>(object)->*Member); },
                [](void *object) noexcept -> void * { return &(static_cast<OwnerType *>(object)->*Member); }};
    }

private: // Member Variables
    std::string_view m_Name;
    Identifier m_Id;
}; // struct MemberField

/** @brief Generate a typed member descriptor at compile time. */
template <auto Member> consteval auto Field(std::string_view name)
{
    static_assert(std::is_member_object_pointer_v<decltype(Member)>);
    return MemberField<Member>{name, Id(name)};
}

template <class T, class... Fields> struct ComponentDescriptor
{
public: // Types
    using FieldsType = std::tuple<Fields...>;
    using InspectionType = std::array<FieldDescriptor, sizeof...(Fields)>;

public: // Methods
    constexpr ComponentDescriptor(Identifier id, std::string_view name, std::uint32_t version,
                                  FieldsType fields, InspectionType inspection) noexcept
        : m_Id(id), m_Name(name), m_Version(version), m_Fields(fields), m_Inspection(inspection)
    {
    }
    constexpr Identifier GetId() const noexcept
    {
        return m_Id;
    }
    constexpr std::string_view GetName() const noexcept
    {
        return m_Name;
    }
    constexpr std::uint32_t GetVersion() const noexcept
    {
        return m_Version;
    }
    constexpr const FieldsType &GetFields() const noexcept
    {
        return m_Fields;
    }
    constexpr const InspectionType &GetInspection() const noexcept
    {
        return m_Inspection;
    }
    constexpr const FieldDescriptor *Find(Identifier fieldId) const noexcept
    {
        for (const auto &entry : m_Inspection)
            if (entry.GetId() == fieldId)
                return &entry;
        return nullptr;
    }

private: // Member Variables
    Identifier m_Id;
    std::string_view m_Name;
    std::uint32_t m_Version;
    FieldsType m_Fields;
    InspectionType m_Inspection;
}; // struct ComponentDescriptor

/** @brief Generate a component schema and reject duplicate field IDs. */
template <class T, class... Fields>
consteval auto Component(std::string_view name, std::uint32_t version, Fields... fields)
{
    static_assert((std::same_as<T, typename Fields::OwnerType> && ...), "Field belongs to another Component");
    if (name.empty() || version == 0)
        throw "Component requires a name and a nonzero Schema version";
    std::array<FieldDescriptor, sizeof...(Fields)> inspection{fields.Erase()...};
    for (std::size_t i = 0; i < inspection.size(); ++i)
    {
        if (inspection[i].GetName().empty())
            throw "Field requires a name";
        for (std::size_t j = 0; j < i; ++j)
            if (inspection[i].GetId() == inspection[j].GetId())
                throw "Duplicate Field Identifier";
    }
    return ComponentDescriptor<T, Fields...>{Id(name), name, version, {fields...}, inspection};
}

/** @brief Return static component metadata without allocation. */
template <Reflected T> constexpr const auto &Describe() noexcept
{
    return Schema<T>::ms_Descriptor;
}
template <Reflected T> inline constexpr Identifier ComponentId = Schema<T>::ms_Descriptor.GetId();
/** @brief Look up a static field descriptor by numeric ID without allocation. */
template <Reflected T> constexpr const FieldDescriptor *FindField(Identifier fieldId) noexcept
{
    return Describe<T>().Find(fieldId);
}
} // namespace vigil

#endif // VIGIL_SCHEMA_HPP
