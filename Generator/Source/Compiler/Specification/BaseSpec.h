/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_BASESPEC_H
#define VIGILSDK_BASESPEC_H

#include <map>
#include <string_view>

#ifdef __MSVCRT__
#define VG_INLINE __forceinline
#else
#define VG_INLINE inline
#endif

namespace vigil::inline parser_specification
{
    /// Typedefs to declare before parsing.
    constexpr std::string_view kTypedefs =
R"(typedef signed char vgChar;
typedef unsigned char vgUChar;
typedef unsigned char vgByte;
typedef unsigned char vgU8;
typedef signed char vgS8;
typedef signed short vgShort;
typedef unsigned short vgUShort;
typedef signed int vgS32;
typedef unsigned int vgU32;
typedef signed long long vgS64;
typedef unsigned long long vgU64;
typedef float vgFloat;
typedef double vgDouble;
typedef bool vgBool;
typedef const char* vgString;

#define VG_CLASS_REFLECTION __attribute__((annotate("VG_CLASS_REFLECTION")))
#define VG_SAFE_FIELD __attribute__((annotate("VG_SAFE_FIELD")))

)";

    const std::map<std::string, std::string> kFieldAnnotationToFlagsMap = {
        {"VG_SAFE_FIELD", "vigil::ClassMember::Flags_SafeField | vigil::ClassMember::Flags_Pointer"},
    };

    constexpr const char* kFieldFlag_None = "vigil::ClassMember::Flags_None";
    constexpr std::string_view kFieldFlag_Pointer = "vigil::ClassMember::Flags_Pointer";
    constexpr std::string_view kFieldFlag_ConstantArray = "vigil::ClassMember::Flags_ConstantArray";

    enum AttributeIndex
    {
        AttributeIndex_ClassReflection = 0,
        AttributeIndex_SafeField = 1,

    }; // enum AttributeIndex

    static const char* kAttributes[] = {
        "VG_CLASS_REFLECTION",
        "VG_SAFE_FIELD",
    };

} // namespace vigil::inline parser_specification

#endif //VIGILSDK_BASESPEC_H
