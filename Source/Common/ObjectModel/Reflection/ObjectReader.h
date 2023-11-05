/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_OBJECTREADER_H
#define VIGILSDK_OBJECTREADER_H

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    class ObjectReader
    {
    public: // Constructors and Destructor
        ObjectReader() = default;
        virtual ~ObjectReader() = default;

    public: // Methods
        virtual vgBool Read(class Object* object, class ClassMember* member) = 0;

    }; // class ObjectReader

} // namespace vigil

#endif //VIGILSDK_OBJECTREADER_H
