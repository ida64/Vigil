/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_BASECLASS_H
#define VIGILSDK_BASECLASS_H

#include <Common/Base/BaseDefs.h>

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
        Class(vgString className, vgU32 classID, Class* parentClass);

        // Default destructor
        virtual ~Class() = default;

    public: // Methods
        // Returns the name of the class
        VG_INLINE vgString GetClassName() const;

        // Returns the unique ID of the class
        VG_INLINE vgU32 GetClassID() const;

        // Returns the parent class
        VG_INLINE Class* GetParentClass() const;

    private: // Member Variables
        // Name of the class (e.g. "Class")
        vgString m_ClassName;
        // Unique ID of the class (e.g. 0x00000000)
        // Typically, unless explicitly specified, this is the CRC32 of the class name
        vgU32 m_ClassID;
        // Pointer to the parent class
        Class* m_ParentClass;

    }; // class Class

    Class::Class(vgString className, vgU32 classID, Class* parentClass)
    : m_ClassName(className), m_ClassID(classID), m_ParentClass(parentClass)
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

} // namespace vigil

#endif //VIGILSDK_BASECLASS_H
