/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_CLASSENUM_H
#define VIGILSDK_CLASSENUM_H

#include <Common/Base/BaseDefs.h>
#include <Common/Container/Array.h>

#include <Common/ObjectModel/Reflection/EnumValue.h>

namespace vigil
{
    /// ClassEnum represents an enum in a class
    class ClassEnum
    {
    public: // Constructors and Destructor
        /// Default constructor
        /// @param [in] name Name of the enum
        /// @param [in] id ID of the enum
        /// @param [in] values Array of enum values
        /// @param [in] nbValues Number of values in the array
        VG_INLINE ClassEnum(vgString name, vgU32 id, const EnumValue* values, vgS32 nbValues);

        /// Default destructor
        ~ClassEnum() = default;

    public: // Methods
        /// Returns the name of the enum
        /// @return [out] m_EnumName
        VG_INLINE vgString GetEnumName() const;

        /// Returns the ID of the enum
        /// @return [out] m_EnumID
        VG_INLINE vgU32 GetEnumID() const;

        /// Returns the number of values in the enum
        /// @return [out] m_EnumValues
        VG_INLINE vgS32 GetNbValues() const;

        /// Returns the value at the given index
        /// @param [in] index Index of the value to return
        /// @return [out] m_EnumValues[index]
        VG_INLINE EnumValue GetEnumValue(vgS32 index) const;

        /// Returns an array of all values in the enum
        /// @return [out] Array of all values in the enum
        VG_INLINE Array<EnumValue> GetEnumValues() const;

    private: // Member Variables
        /// Name of the enum
        vgString m_EnumName;

        /// ID of the enum
        vgU32 m_EnumID;

        /// Array of enum values
        Array<EnumValue> m_EnumValues;

    }; // class ClassEnum

    ClassEnum::ClassEnum(vgString name, vgU32 id, const EnumValue* values, vgS32 nbValues)
            : m_EnumName(name)
            , m_EnumID(id)
    {
        // TODO: Add a copy-from to Array
        m_EnumValues.Resize(nbValues);
        for(vgS32 i = 0; i < nbValues; ++i)
        {
            m_EnumValues[i] = values[i];
        }
    }

    vgString ClassEnum::GetEnumName() const
    {
        return m_EnumName;
    }

    vgU32 ClassEnum::GetEnumID() const
    {
        return m_EnumID;
    }

    vgS32 ClassEnum::GetNbValues() const
    {
        return m_EnumValues.GetSize();
    }

    EnumValue ClassEnum::GetEnumValue(vgS32 index) const
    {
        return m_EnumValues[index];
    }

    Array<EnumValue> ClassEnum::GetEnumValues() const
    {
        return m_EnumValues;
    }

} // namespace vigil

#endif //VIGILSDK_CLASSENUM_H
