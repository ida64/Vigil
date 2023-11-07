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
    /// TypeID represents a unique ID for a type
    enum TypeID : vgU32
    {
        TypeID_Bool = VG_CRC32("bool"),
        TypeID_Char = VG_CRC32("char"),
        TypeID_U32 = VG_CRC32("uint32_t"),
        TypeID_S32 = VG_CRC32("int32_t"),
        TypeID_U64 = VG_CRC32("uint64_t"),
        TypeID_S64 = VG_CRC32("int64_t"),
        TypeID_Float = VG_CRC32("float"),
        TypeID_Double = VG_CRC32("double"),

    }; // enum TypeID

    /// ClassMember represents a member of a class
    class ClassMember
    {
    public: // Constructors and Destructor
        /// Default constructor
        /// @param [in] id Unique ID of the member
        /// @param [in] name Name of the member
        /// @param [in] typeID Unique ID of the member type
        /// @param [in] type Type of the member
        /// @param [in] offset Offset of the member from the start of the class
        /// @param [in] size Size of the member
        /// @param [in] flags Flags of the member
        VG_INLINE ClassMember(vgU32 id, vgString name, vgU32 typeID, vgString type, vgU32 offset, vgU32 size, vgU32 flags = Flags_None);

        /// Default destructor
        virtual ~ClassMember() = default;

    public: // Constants
        enum Flags : vgU32
        {
            Flags_None,
            Flags_Required = 1 << 0,
            Flags_ConstantArray = 1 << 1,
            Flags_Pointer = 1 << 2,

        }; // enum Flags

    public: // Methods
        /// Returns the unique ID of the member
        /// @return [out] m_ID
        VG_INLINE vgU32 GetID() const;

        /// Returns the name of the member
        /// @return [out] m_Name
        VG_INLINE vgString GetName() const;

        /// Returns the unique ID of the member type
        /// @return [out] m_TypeID
        VG_INLINE vgU32 GetTypeID() const;

        /// Returns the type of the member
        /// @return [out] m_Type
        VG_INLINE vgString GetType() const;

        /// Returns the offset of the member from the start of the class
        /// @return [out] m_Offset
        VG_INLINE vgU32 GetOffset() const;

        /// Returns the size of the member
        /// @return [out] m_Size
        VG_INLINE vgU32 GetSize() const;

        /// Returns true if the member is required
        /// @return [out] True if the member is required
        VG_INLINE vgBool IsRequired() const;

        /// Returns true if the member is a constant array
        /// @return [out] True if the member is a constant array
        VG_INLINE vgBool IsConstantArray() const;

        /// Returns true if the member is an object pointer
        /// @return [out] True if the member is an object pointer
        VG_INLINE vgBool IsPointer() const;

    private: // Member Variables
        /// Unique ID of the member
        /// @note Typically, this is the CRC32 of the member name
        vgU32 m_ID;

        /// Name of the member
        vgString m_Name;

        /// Type ID of the member
        /// @note Typically, this is the CRC32 of the member type
        vgU32 m_TypeID;

        /// Type of the member
        vgString m_Type;

        /// Offset of the member from the start of the class
        vgU32 m_Offset;

        /// Size of the member
        vgU32 m_Size;

        /// Flags of the member
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

    vgBool ClassMember::IsRequired() const
    {
        return (m_Flags & Flags_Required) != 0;
    }

    vgBool ClassMember::IsConstantArray() const
    {
        return (m_Flags & Flags_ConstantArray) != 0;
    }

    vgBool ClassMember::IsPointer() const
    {
        return (m_Flags & Flags_Pointer) != 0;
    }

} // namespace vigil

#endif //VIGILSDK_CLASSMEMBER_H
