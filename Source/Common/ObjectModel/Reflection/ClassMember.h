/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_CLASSMEMBER_H
#define VIGILSDK_CLASSMEMBER_H

#include <Common/System/Crc32.h>
#include <Common/Base/BaseDefs.h>

namespace vigil
{
    enum TypeID : vgU32
    {
        TypeID_Bool = VG_CRC32("bool"),
        TypeID_String = VG_CRC32("const char*"),
        TypeID_U8 = VG_CRC32("uint8_t"),
        TypeID_S8 = VG_CRC32("int8_t"),
        TypeID_U16 = VG_CRC32("uint16_t"),
        TypeID_S16 = VG_CRC32("int16_t"),
        TypeID_U32 = VG_CRC32("uint32_t"),
        TypeID_S32 = VG_CRC32("int32_t"),
        TypeID_U64 = VG_CRC32("uint64_t"),
        TypeID_S64 = VG_CRC32("int64_t"),

    }; // enum TypeID

    class ClassMember
    {
    public: // Constructors and Destructor
        // Constructs a class member with the given ID, name, type ID, type, offset, and size
        VG_INLINE ClassMember(vgU32 id, vgString name, vgU32 typeID, vgString type, vgU32 offset, vgU32 size, vgU32 flags = Flags_None);

        // Default destructor
        virtual ~ClassMember() = default;

    public: // Constants
        enum Flags : vgU32
        {
            Flags_None,
            Flags_Required = 1 << 0,

        }; // enum Flags

    public: // Methods
        // Returns the unique ID of the member
        VG_INLINE vgU32 GetID() const;

        // Returns the name of the member
        VG_INLINE vgString GetName() const;

        // Returns the unique ID of the member type
        VG_INLINE vgU32 GetTypeID() const;

        // Returns the type of the member
        VG_INLINE vgString GetType() const;

        // Returns the offset of the member from the start of the class
        VG_INLINE vgU32 GetOffset() const;

        // Returns the size of the member
        VG_INLINE vgU32 GetSize() const;

        VG_INLINE bool IsRequired() const;

    private: // Member Variables
        // Unique ID of the member, typically the CRC32 of the member name
        vgU32 m_ID;
        // Name of the member
        vgString m_Name;
        // Type ID of the member, typically the CRC32 of the member type
        vgU32 m_TypeID;
        // Type of the member
        vgString m_Type;
        // Offset of the member from the start of the class
        vgU32 m_Offset;
        // Size of the member
        vgU32 m_Size;
        // Flags of the member
        vgU32 m_Flags;

    }; // class ClassMember

    VG_INLINE ClassMember::ClassMember(vgU32 id, vgString name, vgU32 typeID, vgString type, vgU32 offset, vgU32 size, vgU32 flags)
    : m_ID(id), m_Name(name), m_TypeID(typeID), m_Type(type), m_Offset(offset), m_Size(size), m_Flags(flags)
    {
    }

    vgU32 ClassMember::GetID() const
    {
        return m_ID;
    }

    vgString ClassMember::GetName() const
    {
        return m_Name;
    }

    vgU32 ClassMember::GetTypeID() const
    {
        return m_TypeID;
    }

    vgString ClassMember::GetType() const
    {
        return m_Type;
    }

    vgU32 ClassMember::GetOffset() const
    {
        return m_Offset;
    }

    vgU32 ClassMember::GetSize() const
    {
        return m_Size;
    }

    bool ClassMember::IsRequired() const
    {
        return (m_Flags & Flags_Required) != 0;
    }

} // namespace vigil

#endif //VIGILSDK_CLASSMEMBER_H
