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
    // Class describes a class in the object model
    class Class
    {
    public: // Constructors and Destructor
        // Constructs a class with the given name, ID, and parent class
        VG_INLINE Class(const ClassMember* members, vgS32 nbMembers, vgString className, vgU32 classID, Class* parentClass);

        // Default destructor
        virtual ~Class() = default;

    public: // Methods
        // Returns the name of the class
        VG_INLINE vgString GetClassName() const;

        // Returns the unique ID of the class
        VG_INLINE vgU32 GetClassID() const;

        // Returns the parent class
        VG_INLINE Class* GetParentClass() const;

        // Returns the number of members in the class
        VG_INLINE vgS32 GetNbMembers() const;

        // Returns the member at the given index
        VG_INLINE ClassMember GetMember(vgS32 index) const;

        VG_INLINE Array<ClassMember> GetMembers() const
        {
            auto arr = Array<ClassMember>(m_NbMembers);
            for(vgS32 i = 0; i < m_NbMembers; ++i)
            {
                arr[i] = m_Members[i];
            }
            return arr;
        }

    private: // Member Variables
        // Array of class members
        const ClassMember* m_Members;
        // Number of members in the class
        vgS32 m_NbMembers;
        // Name of the class (e.g. "Class")
        vgString m_ClassName;
        // Unique ID of the class (e.g. 0x00000000)
        // Typically, unless explicitly specified, this is the CRC32 of the class name
        vgU32 m_ClassID;
        // Pointer to the parent class
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

} // namespace vigil

#endif //VIGILSDK_BASECLASS_H
