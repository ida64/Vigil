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
    else if constexpr (requires(T t) {
                           t.begin();
                           t.end();
                       })
        return FieldKind::Sequence;
    else
        return FieldKind::Opaque;
}

// A plugin/inspector can retain this descriptor for the life of the program.
// Accessors require a valid instance of the descriptor's owning component type.
struct FieldDescriptor
{
public: // Member Variables
    Identifier m_Id;
    std::string_view m_Name;
    FieldKind m_Kind;
    std::size_t m_Size;
    const void *(*m_Read)(const void *) noexcept;
    void *(*m_Write)(void *) noexcept;
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

public: // Member Variables
    std::string_view m_Name;
    Identifier m_Id;

public: // Methods
    constexpr FieldDescriptor Erase() const noexcept
    {
        return {m_Id,
                m_Name,
                KindOf<ValueType>(),
                sizeof(ValueType),
                [](const void *object) noexcept -> const void *
                { return &(static_cast<const OwnerType *>(object)->*Member); },
                [](void *object) noexcept -> void * { return &(static_cast<OwnerType *>(object)->*Member); }};
    }
}; // struct MemberField

/** @brief Generate a typed member descriptor at compile time. */
template <auto Member> consteval auto Field(std::string_view name)
{
    static_assert(std::is_member_object_pointer_v<decltype(Member)>);
    return MemberField<Member>{name, Id(name)};
}

template <class T, class... Fields> struct ComponentDescriptor
{
public: // Member Variables
    Identifier m_Id;
    std::string_view m_Name;
    std::uint32_t m_Version;
    std::tuple<Fields...> m_Fields;
    std::array<FieldDescriptor, sizeof...(Fields)> m_Inspection;

public: // Methods
    constexpr const FieldDescriptor *Find(Identifier fieldId) const noexcept
    {
        for (const auto &entry : m_Inspection)
            if (entry.m_Id == fieldId)
                return &entry;
        return nullptr;
    }
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
        if (inspection[i].m_Name.empty())
            throw "Field requires a name";
        for (std::size_t j = 0; j < i; ++j)
            if (inspection[i].m_Id == inspection[j].m_Id)
                throw "Duplicate Field Identifier";
    }
    return ComponentDescriptor<T, Fields...>{Id(name), name, version, {fields...}, inspection};
}

/** @brief Return static component metadata without allocation. */
template <Reflected T> constexpr const auto &Describe() noexcept
{
    return Schema<T>::ms_Descriptor;
}
template <Reflected T> inline constexpr Identifier ComponentId = Schema<T>::ms_Descriptor.m_Id;
/** @brief Look up a static field descriptor by numeric ID without allocation. */
template <Reflected T> constexpr const FieldDescriptor *FindField(Identifier fieldId) noexcept
{
    return Describe<T>().Find(fieldId);
}
} // namespace vigil

#endif // VIGIL_SCHEMA_HPP
