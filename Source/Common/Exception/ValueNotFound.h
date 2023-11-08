/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_VALUENOTFOUND_H
#define VIGILSDK_VALUENOTFOUND_H

#include <exception>

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    class ValueNotFoundException : public std::exception
    {
    public: // Constructors and Destructor
        /// Default constructor
        explicit ValueNotFoundException(vgString what)
            : m_What(what)
        {
        }

        /// Default destructor
        ~ValueNotFoundException() override = default;

    private: // Member Variables
        vgString m_What;

    }; // class MethodException

} // namespace vigil

#endif //VIGILSDK_VALUENOTFOUND_H
