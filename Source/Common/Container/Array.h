/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_ARRAY_H
#define VIGILSDK_ARRAY_H

#include <cstring>
#include <memory>

#include <Common/Base/BaseDefs.h>
#include <Common/Container/FixedArray.h>

namespace vigil
{
    template<typename T, class Allocator = std::allocator<T>>
    class Array
    {
    public: // Constructors and Destructor
        VG_INLINE Array()
        {
            m_Data = m_Allocator.allocate(0);
            m_Size = 0;
        }

        VG_INLINE Array(std::initializer_list<T> list)
        {
            m_Data = m_Allocator.allocate(list.size());
            m_Size = list.size();

            std::copy(list.begin(), list.end(), m_Data);
        }

        VG_INLINE explicit Array(vgU32 size)
        {
            m_Data = m_Allocator.allocate(size);
            m_Size = size;
        }

        VG_INLINE Array(const Array& other)
        {
            m_Data = m_Allocator.allocate(other.m_Size);
            m_Size = other.m_Size;

            std::copy(other.m_Data, other.m_Data + other.m_Size, m_Data);
        }

        VG_INLINE Array(Array&& other) noexcept
        {
            m_Data = other.m_Data;
            m_Size = other.m_Size;

            other.m_Data = nullptr;
            other.m_Size = 0;
        }

        VG_INLINE ~Array() = default;

    public: // Operators
        T& operator[](vgU32 index)
        {
            return m_Data[index];
        }

        const T& operator[](vgU32 index) const
        {
            return m_Data[index];
        }

        Array& operator=(const FixedArrayBase& other)
        {
            this->Resize(other.m_Size);

            memcpy_s(m_Data, m_Size * sizeof(T), other.m_Data, other.m_Size * sizeof(T));

            return *this;
        }

    public: // Methods
        // Returns the size of the array
        VG_INLINE vgS32 GetSize() const
        {
            return m_Size;
        }

        // Returns the first element of the array
        VG_INLINE T* begin()
        {
            return m_Data;
        }

        // Returns the last element of the array
        VG_INLINE T* end()
        {
            return m_Data + m_Size;
        }

        // Resize the array to the given size
        VG_INLINE void Resize(vgU32 size)
        {
            T* newData = m_Allocator.allocate(size);

            std::copy(m_Data, m_Data + m_Size, newData);

            m_Allocator.deallocate(m_Data, m_Size);

            m_Data = newData;
            m_Size = size;
        }

        // Returns the element at the given index
        VG_INLINE T& Get(vgU32 index)
        {
            return m_Data[index];
        }

        // Returns the element at the given index
        VG_INLINE const T& Get(vgU32 index) const
        {
            return m_Data[index];
        }

        // Adds the given element to the end of the array
        VG_INLINE void PushBack(const T& element)
        {
            Resize(m_Size + 1);
            m_Data[m_Size - 1] = element;
        }

        // Adds the given element to the end of the array
        VG_INLINE void PushBack(T&& element)
        {
            Resize(m_Size + 1);
            m_Data[m_Size - 1] = std::move(element);
        }

        // Removes the last element from the array
        VG_INLINE void PopBack()
        {
            Resize(m_Size - 1);
        }

        // Removes all elements from the array
        VG_INLINE void Clear()
        {
            Resize(0);
        }

    private: // Member Variables
        Allocator m_Allocator;
        T* m_Data;
        vgU32 m_Size;

    }; // class Array

} // namespace vigil

#endif //VIGILSDK_ARRAY_H
