/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_TYPEBUILDER_H
#define VIGILSDK_TYPEBUILDER_H

#include <string>

namespace vigil
{
    class TypeBuilder
    {
    public: // Constructors and Destructor
        /// Default constructor
        TypeBuilder() = default;

        /// Default destructor
        virtual ~TypeBuilder() = default;

    public: // Methods
        /// Build the type into a string
        /// @return The built type
        virtual std::string Build() = 0;

    }; // class TypeBuilder

} // namespace vigil

#endif //VIGILSDK_TYPEBUILDER_H
