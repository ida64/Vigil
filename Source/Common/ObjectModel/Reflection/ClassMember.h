/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_CLASSMEMBER_H
#define VIGILSDK_CLASSMEMBER_H

#include <Common/Base/BaseDefs.h>

#define VG_CLASS_MEMBER_ARRAY(Type) static ClassMember* k##Type##ClassMembers[]

#define VG_CLASS_MEMBER(Type, Name) \
static inline ClassMember ms_##Name##ClassMember = \
{                                          \
    ComputeCrc32(#Name, VG_ARRAY_SIZE(#Name) - 1), \
    #Name, \
    ComputeCrc32(#Type, VG_ARRAY_SIZE(#Type) - 1), \
    #Type, \
    0, \
    sizeof(Type) \
};

namespace vigil
{
    class ClassMember
    {
    public: // Constructors and Destructor
        // Constructs a class member with the given ID, name, type ID, type, offset, and size
        VG_INLINE ClassMember(vgU32 id, vgString name, vgU32 typeID, vgString type, vgU32 offset, vgU32 size);

        // Default destructor
        virtual ~ClassMember() = default;

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

    }; // class ClassMember

    VG_INLINE ClassMember::ClassMember(vgU32 id, vgString name, vgU32 typeID, vgString type, vgU32 offset, vgU32 size)
    : m_ID(id), m_Name(name), m_TypeID(typeID), m_Type(type), m_Offset(offset), m_Size(size)
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

} // namespace vigil

#endif //VIGILSDK_CLASSMEMBER_H
