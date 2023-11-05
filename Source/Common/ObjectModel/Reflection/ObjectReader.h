/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_OBJECTREADER_H
#define VIGILSDK_OBJECTREADER_H

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    /// ObjectReader is an interface for reading objects
    class ObjectReader
    {
    public: // Constructors and Destructor
        /// Default constructor
        ObjectReader() = default;

        /// Default destructor
        virtual ~ObjectReader() = default;

    public: // Methods
        /// Read reads the given member from the given object
        /// @param [in] object Pointer to the object to read from
        /// @param [in] member Pointer to the member to read
        /// @return vgBool True if the member was read successfully, false otherwise
        virtual vgBool Read(class Object* object, class ClassMember* member) = 0;

    }; // class ObjectReader

} // namespace vigil

#endif //VIGILSDK_OBJECTREADER_H
