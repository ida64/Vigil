/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_OBJECTWRITER_H
#define VIGILSDK_OBJECTWRITER_H

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    class ObjectWriter
    {
    public: // Constructors and Destructor
        /// Default constructor
        ObjectWriter() = default;

        /// Default destructor
        virtual ~ObjectWriter() = default;

    public: // Methods
        /// Write writes the given member to the given object
        /// @param [in] object Pointer to the object to write to
        /// @param [in] member Pointer to the member to write
        /// @return vgBool True if the member was written successfully, false otherwise
        virtual vgBool Write(class Object* object, class ClassMember* member) = 0;

    }; // class ObjectWriter

} // namespace vigil

#endif //VIGILSDK_OBJECTWRITER_H
