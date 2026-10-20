#ifndef VIGIL_BINARY_HPP
#define VIGIL_BINARY_HPP

#include "schema.hpp"
#include <bit>
#include <limits>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace vigil
{
enum class BinaryError
{
    None,
    Truncated,
    LimitExceeded,
    InvalidFormat,
    WrongComponent,
    UnsupportedVersion,
    DuplicateField,
    MissingField,
    TrailingData
}; // enum class BinaryError
struct BinaryLimits
{
public: // Member Variables
    std::size_t m_MaxBytes = 16 * 1024 * 1024;
    std::size_t m_MaxSequenceElements = 1024 * 1024;
    std::size_t m_MaxStringBytes = 1024 * 1024;
    std::size_t m_MaxDepth = 32;
    std::size_t m_MaxDecodedBytes = 32 * 1024 * 1024;
}; // struct BinaryLimits
struct BinaryResult
{
public: // Member Variables
    BinaryError m_Error = BinaryError::None;

public: // Methods
    explicit operator bool() const noexcept
    {
        return m_Error == BinaryError::None;
    }
}; // struct BinaryResult

namespace detail
{
struct BinaryFailure
{
public: // Member Variables
    BinaryError m_Error;
}; // struct BinaryFailure
inline void Require(bool condition, BinaryError error)
{
    if (!condition)
        throw BinaryFailure{error};
}
struct Writer
{
public: // Member Variables
    std::vector<std::byte> m_Bytes;
    const BinaryLimits &m_Limits;

public: // Methods
    void Integer(std::uint64_t value, std::size_t width)
    {
        Require(m_Bytes.size() <= m_Limits.m_MaxBytes && width <= m_Limits.m_MaxBytes - m_Bytes.size(),
                BinaryError::LimitExceeded);
        for (std::size_t i = 0; i < width; ++i)
            m_Bytes.push_back(std::byte((value >> (i * 8)) & 0xff));
    }
    void Patch(std::size_t offset, std::uint64_t value)
    {
        for (std::size_t i = 0; i < 4; ++i)
            m_Bytes[offset + i] = std::byte((value >> (i * 8)) & 0xff);
    }
}; // struct Writer
struct Reader
{
public: // Member Variables
    std::span<const std::byte> m_Bytes;
    const BinaryLimits &m_Limits;
    std::size_t m_Offset = 0;
    std::size_t *m_DecodedBytes = nullptr;

public: // Methods
    void Charge(std::size_t count, std::size_t elementSize)
    {
        Require(elementSize == 0 || count <= m_Limits.m_MaxDecodedBytes / elementSize, BinaryError::LimitExceeded);
        const auto bytes = count * elementSize;
        Require(*m_DecodedBytes <= m_Limits.m_MaxDecodedBytes && bytes <= m_Limits.m_MaxDecodedBytes - *m_DecodedBytes,
                BinaryError::LimitExceeded);
        *m_DecodedBytes += bytes;
    }
    std::uint64_t Integer(std::size_t width)
    {
        Require(width <= m_Bytes.size() - m_Offset, BinaryError::Truncated);
        std::uint64_t value = 0;
        for (std::size_t i = 0; i < width; ++i)
            value |= std::uint64_t(std::to_integer<unsigned char>(m_Bytes[m_Offset++])) << (i * 8);
        return value;
    }
    Reader Slice(std::size_t length)
    {
        Require(length <= m_Bytes.size() - m_Offset, BinaryError::Truncated);
        auto result = Reader{m_Bytes.subspan(m_Offset, length), m_Limits, 0, m_DecodedBytes};
        m_Offset += length;
        return result;
    }
}; // struct Reader
template <class T> struct IsVector : std::false_type
{
}; // struct IsVector
template <class T, class A> struct IsVector<std::vector<T, A>> : std::true_type
{
}; // struct IsVector
template <class T> struct IsArray : std::false_type
{
}; // struct IsArray
template <class T, std::size_t N> struct IsArray<std::array<T, N>> : std::true_type
{
}; // struct IsArray
template <class> inline constexpr bool Unsupported = false;

template <class T> void Encode(Writer &out, const T &value, std::size_t depth)
{
    Require(depth <= out.m_Limits.m_MaxDepth, BinaryError::LimitExceeded);
    if constexpr (std::same_as<T, bool>)
        out.Integer(value ? 1 : 0, 1);
    else if constexpr (std::is_integral_v<T>)
    {
        static_assert(sizeof(T) <= 8);
        out.Integer(static_cast<std::make_unsigned_t<T>>(value), sizeof(T));
    }
    else if constexpr (std::is_enum_v<T>)
        Encode(out, static_cast<std::underlying_type_t<T>>(value), depth);
    else if constexpr (std::is_floating_point_v<T>)
    {
        static_assert((sizeof(T) == 4 || sizeof(T) == 8) && std::numeric_limits<T>::is_iec559);
        if constexpr (sizeof(T) == 4)
            out.Integer(std::bit_cast<std::uint32_t>(value), 4);
        else
            out.Integer(std::bit_cast<std::uint64_t>(value), 8);
    }
    else if constexpr (std::same_as<T, std::string>)
    {
        Require(value.size() <= out.m_Limits.m_MaxStringBytes && value.size() <= UINT32_MAX,
                BinaryError::LimitExceeded);
        out.Integer(value.size(), 4);
        for (unsigned char c : value)
            out.Integer(c, 1);
    }
    else if constexpr (IsVector<T>::value || IsArray<T>::value)
    {
        Require(value.size() <= out.m_Limits.m_MaxSequenceElements && value.size() <= UINT32_MAX,
                BinaryError::LimitExceeded);
        out.Integer(value.size(), 4);
        for (const auto &item : value)
        {
            if constexpr (std::same_as<typename T::value_type, bool>)
                Encode(out, static_cast<bool>(item), depth + 1);
            else
                Encode(out, item, depth + 1);
        }
    }
    else if constexpr (Reflected<T>)
    {
        const auto &descriptor = Describe<T>();
        out.Integer(0x43534756, 4); // VGSC
        out.Integer(descriptor.m_Id, 8);
        out.Integer(descriptor.m_Version, 4);
        out.Integer(descriptor.m_Inspection.size(), 4);
        std::apply(
            [&](const auto &...fields)
            {
                (
                    [&]
                    {
                        out.Integer(fields.m_Id, 8);
                        auto lengthOffset = out.m_Bytes.size();
                        out.Integer(0, 4);
                        auto start = out.m_Bytes.size();
                        Encode(out, value.*(fields.ms_Pointer), depth + 1);
                        Require(out.m_Bytes.size() - start <= UINT32_MAX, BinaryError::LimitExceeded);
                        out.Patch(lengthOffset, out.m_Bytes.size() - start);
                    }(),
                    ...);
            },
            descriptor.m_Fields);
    }
    else
        static_assert(Unsupported<T>, "No portable binary codec for this type");
}

template <class T> void Decode(Reader &in, T &value, std::size_t depth)
{
    Require(depth <= in.m_Limits.m_MaxDepth, BinaryError::LimitExceeded);
    if constexpr (std::same_as<T, bool>)
    {
        auto raw = in.Integer(1);
        Require(raw <= 1, BinaryError::InvalidFormat);
        value = raw != 0;
    }
    else if constexpr (std::is_integral_v<T>)
    {
        static_assert(sizeof(T) <= 8);
        auto raw = static_cast<std::make_unsigned_t<T>>(in.Integer(sizeof(T)));
        value = std::bit_cast<T>(raw);
    }
    else if constexpr (std::is_enum_v<T>)
    {
        std::underlying_type_t<T> raw{};
        Decode(in, raw, depth);
        value = static_cast<T>(raw);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        static_assert((sizeof(T) == 4 || sizeof(T) == 8) && std::numeric_limits<T>::is_iec559);
        if constexpr (sizeof(T) == 4)
            value = std::bit_cast<T>(static_cast<std::uint32_t>(in.Integer(4)));
        else
            value = std::bit_cast<T>(in.Integer(8));
    }
    else if constexpr (std::same_as<T, std::string>)
    {
        auto length = in.Integer(4);
        Require(length <= in.m_Limits.m_MaxStringBytes, BinaryError::LimitExceeded);
        auto data = in.Slice(length);
        in.Charge(length, sizeof(char));
        value.assign(reinterpret_cast<const char *>(data.m_Bytes.data()), data.m_Bytes.size());
    }
    else if constexpr (IsVector<T>::value || IsArray<T>::value)
    {
        auto count = in.Integer(4);
        Require(count <= in.m_Limits.m_MaxSequenceElements, BinaryError::LimitExceeded);
        if constexpr (IsVector<T>::value)
        {
            in.Charge(count, sizeof(typename T::value_type));
            value.clear();
            value.reserve(count);
        }
        else
            Require(count == value.size(), BinaryError::InvalidFormat);
        for (std::size_t i = 0; i < count; ++i)
        {
            typename T::value_type item{};
            Decode(in, item, depth + 1);
            if constexpr (IsVector<T>::value)
                value.push_back(std::move(item));
            else
                value[i] = std::move(item);
        }
    }
    else if constexpr (Reflected<T>)
    {
        const auto &descriptor = Describe<T>();
        Require(in.Integer(4) == 0x43534756, BinaryError::InvalidFormat);
        Require(in.Integer(8) == descriptor.m_Id, BinaryError::WrongComponent);
        Require(in.Integer(4) == descriptor.m_Version, BinaryError::UnsupportedVersion);
        auto count = in.Integer(4);
        Require(count == descriptor.m_Inspection.size(), BinaryError::InvalidFormat);
        std::array<bool, std::tuple_size_v<decltype(descriptor.m_Fields)>> seen{};
        for (std::size_t i = 0; i < count; ++i)
        {
            auto fieldId = in.Integer(8);
            auto length = in.Integer(4);
            auto payload = in.Slice(length);
            bool found = false;
            std::size_t index = 0;
            std::apply(
                [&](const auto &...fields)
                {
                    (
                        [&]
                        {
                            if (fields.m_Id == fieldId)
                            {
                                Require(!seen[index], BinaryError::DuplicateField);
                                seen[index] = true;
                                found = true;
                                Decode(payload, value.*(fields.ms_Pointer), depth + 1);
                                Require(payload.m_Offset == payload.m_Bytes.size(), BinaryError::TrailingData);
                            }
                            ++index;
                        }(),
                        ...);
                },
                descriptor.m_Fields);
            Require(found, BinaryError::InvalidFormat);
        }
        for (bool present : seen)
            Require(present, BinaryError::MissingField);
    }
    else
        static_assert(Unsupported<T>, "No portable binary codec for this type");
}
} // namespace detail

/** @brief Encode portable versioned component data; output is unchanged on failure. */
// Allocation failures propagate.
template <Reflected T> BinaryResult Serialize(const T &value, std::vector<std::byte> &output, BinaryLimits limits = {})
{
    try
    {
        detail::Writer out{{}, limits};
        detail::Encode(out, value, 0);
        output = std::move(out.m_Bytes);
        return {};
    }
    catch (const detail::BinaryFailure &failure)
    {
        return {failure.m_Error};
    }
}
/** @brief Decode bounded component data transactionally, rejecting schema mismatches. */
template <Reflected T> BinaryResult Deserialize(std::span<const std::byte> bytes, T &value, BinaryLimits limits = {})
{
    static_assert(std::is_nothrow_move_assignable_v<T>, "Transactional Decode requires noexcept move assignment");
    if (bytes.size() > limits.m_MaxBytes)
        return {BinaryError::LimitExceeded};
    try
    {
        T candidate{};
        std::size_t decodedBytes = 0;
        detail::Reader in{bytes, limits, 0, &decodedBytes};
        detail::Decode(in, candidate, 0);
        detail::Require(in.m_Offset == bytes.size(), BinaryError::TrailingData);
        value = std::move(candidate);
        return {};
    }
    catch (const detail::BinaryFailure &failure)
    {
        return {failure.m_Error};
    }
}
} // namespace vigil

#endif // VIGIL_BINARY_HPP
