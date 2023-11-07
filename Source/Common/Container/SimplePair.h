/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_SIMPLEPAIR_H
#define VIGILSDK_SIMPLEPAIR_H

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    template<typename First, typename Second>
    class SimplePair
    {
    public: // Typedefs
        /// First member type
        typedef First first_type;

        /// Second member type
        typedef Second second_type;

    public: // Constructors and Destructor
        /// Default constructor
        VG_INLINE SimplePair() = default;

        /// Default destructor
        VG_INLINE ~SimplePair() = default;

        /// Constructor
        /// @param [in] first First member of the pair
        /// @param [in] second Second member of the pair
        VG_INLINE SimplePair(const First& first, const Second& second);

    public: // Methods
        /// GetFirst returns the first member of the pair
        /// @return [out] m_First
        VG_INLINE First GetFirst() const;

        /// GetSecond returns the second member of the pair
        /// @return [out] m_Second
        VG_INLINE Second GetSecond() const;

    private: // Member Variables
        /// First member of the pair
        First m_First;

        /// Second member of the pair
        Second m_Second;

    }; // class SimplePair

    template <typename First, typename Second>
    Second SimplePair<First, Second>::GetSecond() const
    {
        return m_Second;
    }

    template <typename First, typename Second>
    First SimplePair<First, Second>::GetFirst() const
    {
        return m_First;
    }

    template <typename First, typename Second>
    SimplePair<First, Second>::SimplePair(const First& first, const Second& second)
        : m_First(first)
        , m_Second(second)
    {
    }

} // namespace vigil

#endif //VIGILSDK_SIMPLEPAIR_H
