/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_BASECLASS_H
#define VIGILSDK_BASECLASS_H

#include <iostream>
#include <algorithm>

#include <Common/Exception/ValueNotFound.h>

#include <Common/Base/BaseDefs.h>
#include <Common/Container/Array.h>
#include <Common/Container/FixedArray.h>

#include <Common/ObjectModel/Reflection/ClassMember.h>
#include <Common/ObjectModel/Reflection/ClassEnum.h>
#include <Common/ObjectModel/Reflection/ClassMethod.h>

#include <map>

#ifdef GetClassName
#undef GetClassName
#endif

namespace vigil
{
    /// Class stores information about a class and its members
    class Class
    {
    public: // Constructors and Destructor
        /// Default constructor
        /// @param [in] members Array of class members
        /// @param [in] nbMembers Number of members in the array
        /// @param [in] enums Array of class enums
        /// @param [in] className Name of the class
        /// @param [in] classID Unique ID of the class
        /// @param [in] parentClass Pointer to the parent class
        VG_INLINE Class(const ClassMember* members, vgU32 nbMembers,
                        const ClassMethod* methods, vgU32 nbMethods,
                        const FixedArrayBase& enums,
                        vgString className, vgU32 classID,
                        Class* parentClass,
                        void* constructor);

        /// Default destructor
        virtual ~Class() = default;

    public: // Static
        static inline std::map<vgU32, Class*> ms_IDToClassMap;

    public: // Methods
        /// GetClassName returns the name of the class
        /// @return [out] m_ClassName
        VG_INLINE vgString GetClassName() const;

        /// GetClassID returns the unique ID of the class
        /// @return [out] m_ClassID
        VG_INLINE vgU32 GetClassID() const;

        VG_INLINE void* Create() const;

        /// GetParentClass returns the parent class
        /// @return [out] m_ParentClass
        VG_INLINE Class* GetParentClass() const;

        /// GetMembers returns an array of all members in the class
        /// @return [out] Array of all members in the class
        VG_INLINE Array<ClassMember*> GetMembers() const;

        VG_INLINE Array<ClassMethod*> GetMethods() const;

        template <typename Ret, typename... Args>
        Ret CallMethod(vgString name, Args... args);

    private: // Member Variables
        /// Array of class members
        Array<ClassMember*> m_Members;

        /// Array of class enums
        Array<ClassEnum*> m_Enums;

        /// Array of class methods
        Array<ClassMethod*> m_Methods;

        /// Name of the class (e.g. "Class")
        vgString m_ClassName;

        /// Unique ID of the class (e.g. 0x00000000)
        /// @note Typically, unless explicitly specified, this is the CRC32 of the class name
        vgU32 m_ClassID;

        /// Pointer to the constructor of the class
        void*(*m_Constructor)();

        /// Pointer to the parent class
        /// @note If the class has no parent, this is nullptr
        Class* m_ParentClass;

    }; // class Class

    VG_INLINE Class::Class(const ClassMember* members, vgU32 nbMembers,
                           const ClassMethod* methods, vgU32 nbMethods,
                           const FixedArrayBase& enums,
                           vgString className, vgU32 classID,
                           Class* parentClass,
                           void* constructor)
    : m_Constructor(reinterpret_cast<void* (*)()>(constructor)),
    m_ClassName(className), m_ClassID(classID), m_ParentClass(parentClass)
    {
        // Store pointers from the members const array.
        Array<ClassMember*>& membersArray = m_Members;
        membersArray.Resize(nbMembers);

        for(vgS32 idx = 0; auto& ref : membersArray)
        {
            ref = const_cast<ClassMember*>(&members[idx]);
            ++idx;
        }

        // Store pointers from the methods const array.
        Array<ClassMethod*>& methodsArray = m_Methods;
        methodsArray.Resize(nbMethods);

        for(vgS32 idx = 0; auto& ref : methodsArray)
        {
            ref = const_cast<ClassMethod*>(&methods[idx]);
            ++idx;
        }

        m_Enums = enums;
        ms_IDToClassMap[classID] = this;
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

    Array<ClassMember*> Class::GetMembers() const
    {
        return m_Members;
    }

    void* Class::Create() const
    {
        return m_Constructor();
    }

    Array<ClassMethod*> Class::GetMethods() const
    {
        return m_Methods;
    }

    template <typename Ret, typename... Args>
    Ret Class::CallMethod(vgString name, Args... args)
    {
        Array<ClassMethod*> methods = this->GetMethods();

        const vgU32 crc32 = ComputeStringCRC32(name);

        auto iter = std::find_if(methods.begin(), methods.end(),
                                 [crc32](ClassMethod* method) -> bool
                                 {
                                     return method->GetID() == crc32;
                                 });

        if(iter != methods.end())
        {
            return (*iter)->template Call<Ret, Args...>(args...);
        }

        throw ValueNotFoundException("Method not found");
    }

    /// GetClassByID returns the class with the given ID
    /// @param [in] id ID of the class to get
    /// @return Class* Pointer to the class with the given ID
    VG_INLINE Class* GetClassByID(vgU32 id)
    {
        auto iter = Class::ms_IDToClassMap.find(id);
        if(iter != Class::ms_IDToClassMap.end())
        {
            return iter->second;
        }
        return nullptr;
    }

} // namespace vigil

#endif //VIGILSDK_BASECLASS_H
