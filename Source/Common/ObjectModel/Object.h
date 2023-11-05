/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_OBJECT_H
#define VIGILSDK_OBJECT_H

#include <Common/ObjectModel/Reflection/Class.h>
#include <Common/ObjectModel/Reflection/ClassMember.h>

#include <Common/ObjectModel/Reflection/ObjectReader.h>

#include <Common/System/Crc32.h>

// This macro is used to implement the GetClass() method for a class
#define VG_REFLECTED_IMPL(Type) \
Class g_##Type##Class(k##Type##ClassMembers, VG_ARRAY_SIZE(k##Type##ClassMembers), #Type, ComputeCrc32(#Type, VG_ARRAY_SIZE(#Type) - 1), nullptr); \
Class* Type::GetClass() const \
{ \
    return &g_##Type##Class; \
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

    public: // Methods
        /// GetPtrTo returns a pointer to the given member
        /// @param [in] member Pointer to the member to get a pointer to
        /// @return void* Pointer to the member
        VG_INLINE void* GetPtrTo(ClassMember* member);

        /// GetClass returns the class of the object
        /// @return Class* Pointer to the class of the object
        /// @note This method must be implemented by the derived class, see VG_REFLECTED_IMPL
        virtual Class* GetClass() const = 0;

    }; // class Object

} // namespace vigil

#endif //VIGILSDK_OBJECT_H
