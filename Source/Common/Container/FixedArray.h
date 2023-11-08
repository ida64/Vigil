/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_FIXEDARRAY_H
#define VIGILSDK_FIXEDARRAY_H

#include <Common/Base/BaseDefs.h>
#include <array>

namespace vigil
{
    struct FixedArrayBase
    {
        VG_INLINE FixedArrayBase(vgU32 size, const vgByte* data)
            : m_Size(size)
            , m_Data(data)
        {
        }

        /// Size of the array
        vgU32 m_Size;
        /// Size of the array
        const vgByte* m_Data;

    }; // struct FixedArrayBase

    template<typename T, vgU32 N>
    class FixedArray
    {
    public: // Constructors and Destructor
        VG_INLINE FixedArray()
            : m_Base(N, reinterpret_cast<const vgByte*>(m_Data))
        {
        }

        VG_INLINE FixedArray(std::initializer_list<T> list)
            : m_Base(list.size(), reinterpret_cast<const vgByte*>(m_Data))
        {
            vgU32 i = 0;
            for(const T& item : list)
            {
                m_Data[i++] = item;
            }
        }

        VG_INLINE explicit FixedArray(vgU32 size)
            : m_Base(size, reinterpret_cast<const vgByte*>(m_Data))
        {
        }

        VG_INLINE FixedArray(const FixedArray& other)
            : m_Base(other.m_Size, reinterpret_cast<const vgByte*>(m_Data))
        {
            for(vgU32 i = 0; i < this->GetSize(); ++i)
            {
                m_Data[i] = other.m_Data[i];
            }
        }

        VG_INLINE FixedArray(FixedArray&& other) noexcept
            : m_Base(other.m_Size, reinterpret_cast<const vgByte*>(m_Data))
        {
            for(vgU32 i = 0; i < this->GetSize(); ++i)
            {
                m_Data[i] = other.m_Data[i];
            }
        }

        VG_INLINE FixedArray(const T* data, vgU32 size)
            : m_Base(size, reinterpret_cast<const vgByte*>(m_Data))
        {
            for(vgU32 i = 0; i < this->GetSize(); ++i)
            {
                m_Data[i] = data[i];
            }
        }

        VG_INLINE ~FixedArray() = default;

    public: // Methods
        VG_INLINE const FixedArrayBase& GetBase() const
        {
            return m_Base;
        }

        VG_INLINE vgU32 GetSize() const
        {
            return m_Base.m_Size;
        }

        // = Operator so you can do = {1, 2, 3, 4, 5}
        VG_INLINE const FixedArray& operator=(std::initializer_list<T> list)
        {
            vgU32 i = 0;
            for(const T& item : list)
            {
                m_Data[i++] = item;
            }

            return *this;
        }

        VG_INLINE T& operator[](vgU32 index)
        {
            return m_Data[index];
        }

        VG_INLINE const T& operator[](vgU32 index) const
        {
            return m_Data[index];
        }

        VG_INLINE T* GetData()
        {
            return m_Data;
        }

        VG_INLINE const T* GetData() const
        {
            return m_Data;
        }

        VG_INLINE void PushBack(const T& item)
        {
            const auto& base = m_Base;
            const auto size = base.m_Size;
            if(size < N)
            {
                m_Data[size] = item;
                m_Base.m_Size = size + 1;
            }
        }

        VG_INLINE void PopBack()
        {
            const auto& base = m_Base;
            const auto size = base.m_Size;
            if(size > 0)
            {
                m_Base.m_Size = size - 1;
            }
        }

        VG_INLINE T& Front()
        {
            return m_Data[0];
        }

        VG_INLINE const T& Front() const
        {
            return m_Data[0];
        }

        VG_INLINE T& Back()
        {
            return m_Data[m_Base.m_Size - 1];
        }

        VG_INLINE const T& Back() const
        {
            return m_Data[m_Base.m_Size - 1];
        }

        VG_INLINE T* begin()
        {
            return m_Data;
        }

        VG_INLINE const T* begin() const
        {
            return m_Data;
        }

        VG_INLINE T* end()
        {
            return m_Data + m_Base.m_Size;
        }

        VG_INLINE const T* end() const
        {
            return m_Data + m_Base.m_Size;
        }

    private: // Member Variables
        FixedArrayBase m_Base;

        /// Data of the array
        T m_Data[N];

    }; // class FixedArray

} // namespace vigil

#endif //VIGILSDK_FIXEDARRAY_H
