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


    class Object
    {
    public: // Constructors and Destructor
        Object() = default;
        virtual ~Object() = default;

    public: // Static
        static bool Deserialize(const ObjectPtr& object, ObjectReader& reader);

    public: // Methods
        void* GetPtrTo(ClassMember* member)
        {
            return reinterpret_cast<void*>(reinterpret_cast<vgU8*>(this) + member->GetOffset());
        }

        // Returns the class of the object
        virtual Class* GetClass() const = 0;

    }; // class Object

} // namespace vigil

#endif //VIGILSDK_OBJECT_H
