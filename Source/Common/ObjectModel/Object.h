/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_OBJECT_H
#define VIGILSDK_OBJECT_H

#include <Common/ObjectModel/Class.h>
#include <Common/System/Crc32.h>

// This macro is used to implement the GetClass() method for a class
// It is used in the .cpp file of the class
#define VG_REFLECTED_IMPL(Type) \
Class g_##Type##Class(#Type, ComputeCrc32(#Type, VG_ARRAY_SIZE(#Type) - 1), nullptr); \
Class* Type::GetClass() const \
{ \
    return &g_##Type##Class; \
}

namespace vigil
{
    class Object
    {
    public: // Constructors and Destructor
        Object() = default;
        virtual ~Object() = default;

    public: // Methods
        virtual Class* GetClass() const = 0;

    }; // class Object

} // namespace vigil

#endif //VIGILSDK_OBJECT_H
