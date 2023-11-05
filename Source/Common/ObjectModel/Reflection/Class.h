/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_BASECLASS_H
#define VIGILSDK_BASECLASS_H

#include <Common/Base/BaseDefs.h>
#include <Common/Container/Array.h>
#include <Common/ObjectModel/Reflection/ClassMember.h>

#ifdef GetClassName
#undef GetClassName
#endif

namespace vigil
{
    /// Class stores information about a class and its members
    class Class
    {
    public: // Constructors and Destructor
        /// Constructs a class with the given name, ID, and parent class
        /// @param [in] members Array of class members
        /// @param [in] nbMembers Number of members in the class
        /// @param [in] className Name of the class
        /// @param [in] classID Unique ID of the class
        /// @param [in] parentClass Pointer to the parent class
        VG_INLINE Class(const ClassMember* members, vgS32 nbMembers, vgString className, vgU32 classID, Class* parentClass);

        /// Default destructor
        virtual ~Class() = default;

    public: // Methods
        /// GetClassName returns the name of the class
        /// @return [out] m_ClassName
        VG_INLINE vgString GetClassName() const;

        /// GetClassID returns the unique ID of the class
        /// @return [out] m_ClassID
        VG_INLINE vgU32 GetClassID() const;

        /// GetParentClass returns the parent class
        /// @return [out] m_ParentClass
        VG_INLINE Class* GetParentClass() const;

        /// GetNbMembers returns the number of members in the class
        /// @return [out] m_NbMembers
        VG_INLINE vgS32 GetNbMembers() const;

        /// GetMember returns the member at the given index
        /// @param [in] index Index of the member to return
        /// @return [out] m_Members[index]
        VG_INLINE ClassMember GetMember(vgS32 index) const;

        /// GetMembers returns an array of all members in the class
        /// @return [out] Array of all members in the class
        VG_INLINE Array<ClassMember> GetMembers() const;

    private: // Member Variables
        /// Array of class members
        const ClassMember* m_Members;

        /// Number of members in the class
        vgS32 m_NbMembers;

        /// Name of the class (e.g. "Class")
        vgString m_ClassName;

        /// Unique ID of the class (e.g. 0x00000000)
        /// @note Typically, unless explicitly specified, this is the CRC32 of the class name
        vgU32 m_ClassID;

        /// Pointer to the parent class
        /// @note If the class has no parent, this is nullptr
        Class* m_ParentClass;

    }; // class Class

    VG_INLINE Class::Class(const ClassMember* members, vgS32 nbMembers, vgString className, vgU32 classID, Class* parentClass)
    : m_Members(members), m_NbMembers(nbMembers), m_ClassName(className), m_ClassID(classID), m_ParentClass(parentClass)
    {
    }

    vgString Class::GetClassName() const
    {
        return m_ClassName;
    }

    vgU32 Class::GetClassID() const
    {
        return m_ClassID;
    }

    Class* Class::GetParentClass() const
    {
        return m_ParentClass;
    }

    vgS32 Class::GetNbMembers() const
    {
        return m_NbMembers;
    }

    ClassMember Class::GetMember(vgS32 index) const
    {
        return m_Members[index];
    }

    Array<ClassMember> Class::GetMembers() const
    {
        auto arr = Array<ClassMember>(m_NbMembers);
        for(vgS32 i = 0; i < m_NbMembers; ++i)
        {
            arr[i] = m_Members[i];
        }
        return arr;
    }

} // namespace vigil

#endif //VIGILSDK_BASECLASS_H
