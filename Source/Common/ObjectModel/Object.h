/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_OBJECT_H
#define VIGILSDK_OBJECT_H

#include <Common/ObjectModel/Reflection/Class.h>
#include <Common/ObjectModel/Reflection/ClassMember.h>
#include <Common/ObjectModel/Reflection/ClassEnum.h>

#include <Common/ObjectModel/Reflection/ObjectReader.h>

#include <Common/System/Crc32.h>
#include "Common/ObjectModel/Reflection/ObjectWriter.h"

/// VG_REFLECTED_IMPL implements the GetClass method for the given type
#define VG_REFLECTED_IMPL(Type) \
    static Type* s##Type##Constructor() \
    { \
        return new Type(); \
    } \
    static inline Class k##Type##Class( \
            k##Type##ClassMembers, \
            VG_ARRAY_SIZE(k##Type##ClassMembers), \
            k##Type##Enums.GetBase(), \
            #Type, \
            VG_CRC32(#Type), \
            nullptr, \
            (void*)s##Type##Constructor \
            ); \
    Class* Type::GetClass() const \
    { \
        return &k##Type##Class; \
    }

namespace vigil
{
    typedef std::shared_ptr<class Object> ObjectPtr;

    /// Object is the base class for all objects
    class Object
    {
    public: // Constructors and Destructor
        /// Default constructor
        Object() = default;

        /// Default destructor
        virtual ~Object() = default;

    public: // Static
        /// Deserialize deserializes the given object from the given reader
        /// @param [in] object Pointer to the object to deserialize
        /// @param [in] reader Pointer to the reader to deserialize from
        /// @return vgBool True if the object was deserialized successfully, false otherwise
        static bool Deserialize(const ObjectPtr& object, ObjectReader& reader);

        /// Serialize serializes the given object to the given writer
        /// @param [in] object Pointer to the object to serialize
        /// @param [in] writer Pointer to the writer to serialize to
        /// @return vgBool True if the object was serialized successfully, false otherwise
        static bool Serialize(const ObjectPtr& object, ObjectWriter& writer);

    public: // Methods
        /// GetPtrTo returns a pointer to the given member
        /// @param [in] member Pointer to the member to get a pointer to
        /// @return void* Pointer to the member
        void* GetPtrTo(ClassMember* member);

        /// GetValueOf returns the value of the member with the given name
        /// @param [in] name Name of the member to get the value of
        /// @return T Value of the member
        template<typename T>
        T GetValueOf(const vgString& name) const;

        /// SetValueOf sets the value of the member with the given name
        /// @param [in] name Name of the member to set the value of
        /// @param [in] value Value to set the member to
        template<typename T>
        void SetValueOf(const vgString& name, const T& value);

        /// GetClass returns the class of the object
        /// @return Class* Pointer to the class of the object
        /// @note This method must be implemented by the derived class, see VG_REFLECTED_IMPL
        virtual Class* GetClass() const = 0;

    }; // class Object

    // TODO(GetValueOf, SetValueOf): Add proper recursive search

    template <typename T>
    T vigil::Object::GetValueOf(vgString const& name) const
    {
        Class* currentClass = GetClass();
        while(currentClass != nullptr)
        {
            for(auto& classMember : currentClass->GetMembers())
            {
                if(classMember->GetName() == name)
                {
                    return *reinterpret_cast<T*>(reinterpret_cast<vgU8*>(const_cast<Object*>(this)) + classMember->GetOffset());
                }
            }
            currentClass = currentClass->GetParentClass();
        }

        return T();
    }

    template <typename T>
    void vigil::Object::SetValueOf(vgString const& name, const T& value)
    {
        Class* currentClass = GetClass();
        while(currentClass != nullptr)
        {
            for(auto& classMember : currentClass->GetMembers())
            {
                if(classMember->GetName() == name)
                {
                    *reinterpret_cast<T*>(reinterpret_cast<vgU8*>(const_cast<Object*>(this)) + classMember->GetOffset()) = value;
                    return;
                }
            }
            currentClass = currentClass->GetParentClass();
        }
    }

} // namespace vigil

#endif //VIGILSDK_OBJECT_H
