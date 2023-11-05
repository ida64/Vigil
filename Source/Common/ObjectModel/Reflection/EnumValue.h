/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_ENUMVALUE_H
#define VIGILSDK_ENUMVALUE_H

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    class EnumValue
    {
    public: // Constructors and Destructor
        /// Default constructor
        EnumValue(vgString name, vgU32 id, vgU32 value)
            : m_Name(name)
            , m_ID(id)
            , m_Value(value)
        {
        }

        /// Default destructor
        ~EnumValue() = default;

    public: // Methods
        /// Returns the name of the enum value
        /// @return [out] m_Name
        VG_INLINE vgString GetName() const;

        // Returns the ID of the enum value
        /// @return [out] m_ID
        VG_INLINE vgU32 GetID() const;

        /// Returns the value of the enum value
        /// @return [out] m_Value
        VG_INLINE vgU64 GetValue() const;

    private: // Member Variables
        /// Name of the enum value
        vgString m_Name;

        // ID of the enum value
        vgU32 m_ID;

        /// Value of the enum value
        vgU32 m_Value;

    }; // class EnumValue

    vgString EnumValue::GetName() const
    {
        return m_Name;
    }

    vgU64 EnumValue::GetValue() const
    {
        return m_Value;
    }

    vgU32 EnumValue::GetID() const
    {
        return m_ID;
    }

} // namespace vigil

#endif //VIGILSDK_ENUMVALUE_H
