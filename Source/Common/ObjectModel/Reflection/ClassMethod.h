/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_CLASSMETHOD_H
#define VIGILSDK_CLASSMETHOD_H

#include <Common/Base/BaseDefs.h>

namespace vigil
{
    enum MethodType : vgS32
    {
        /// Method is virtual
        MethodType_Virtual,

        /// Method is static
        MethodType_Static,

    }; // enum MethodType

    class ClassMethod
    {
    public: // Constructors and Destructor
        /// Default constructor
        /// @param [in] name Name of the method
        /// @param [in] id Unique ID of the method
        /// @param [in] method Pointer to the method
        /// @param [in] type Type of the method
        VG_INLINE ClassMethod(const vgString& name, vgU32 id, void* method, MethodType type)
            : m_Name(name), m_ID(id), m_Method(method), m_MethodType(type)
        {
        }

        /// Default destructor
        VG_INLINE ~ClassMethod() = default;

    public: // Operators
        bool operator==(const vgU32 id) const
        {
            return m_ID == id;
        }

    public: // Methods
        /// GetName returns the name of the method
        /// @return [out] m_Name
        VG_INLINE vgString GetName() const;

        /// GetID returns the unique ID of the method
        /// @return [out] m_ID
        VG_INLINE vgU32 GetID() const;

        /// GetMethod returns the pointer to the method
        /// @return [out] m_Method
        VG_INLINE void* GetMethod() const;

        /// GetMethodType returns the type of the method
        /// @return [out] m_MethodType
        VG_INLINE MethodType GetMethodType() const;

        /// Call calls the method
        /// @param [in] args Arguments to pass to the method
        /// @return [out] Return value of the method
        template<typename Ret, typename... Args>
        VG_INLINE Ret Call(Args... args) const;

    private: // Member Variables
        /// Name of the method
        vgString m_Name;

        // Unique ID of the method
        vgU32 m_ID;

        /// Pointer to the method
        void* m_Method;

        /// Type of the method
        MethodType m_MethodType;

    }; // class ClassMethod

    vgString ClassMethod::GetName() const
    {
        return m_Name;
    }

    vgU32 ClassMethod::GetID() const
    {
        return m_ID;
    }

    void* ClassMethod::GetMethod() const
    {
        return m_Method;
    }

    MethodType ClassMethod::GetMethodType() const
    {
        return m_MethodType;
    }

    template <typename Ret, typename... Args>
    Ret ClassMethod::Call(Args... args) const

    {
        typedef Ret(*Method)(Args...);
        auto method = reinterpret_cast<Method>(m_Method);
        return method(args...);
    }

} // namespace vigil

#endif //VIGILSDK_CLASSMETHOD_H
