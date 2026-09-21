/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#include "JsonReader.h"

#include <algorithm>
#include <iostream>

template <typename T>
bool Read(vigil::Object* object, vigil::ClassMember* member, const nlohmann::json& m_JsonDoc)
{
    auto* ptr = reinterpret_cast<T*>(object->GetPtrTo(member));
    if(!ptr)
    {
        return false;
    }

    if(member->IsConstantArray())
    {
        if(member->GetSize() % sizeof(T) != 0)
        {
            return false;
        }

        const auto capacity = member->GetSize() / sizeof(T);
        const auto array = m_JsonDoc[member->GetName()].get<std::vector<T>>();
        if(array.size() > capacity)
        {
            return false;
        }

        std::copy(array.begin(), array.end(), ptr);
        std::fill(ptr + array.size(), ptr + capacity, T{});
        return true;
    }

    *ptr = m_JsonDoc[member->GetName()].get<T>();
    return true;
}

vigil::JsonReader::JsonReader(const nlohmann::json& json)
        : m_JsonDoc(json)
{
}

vgBool vigil::JsonReader::Read(Object* object, ClassMember* member)
{
    vgString memberName = member->GetName();

    if (!m_JsonDoc.contains(memberName))
    {
        return false;
    }

    switch(member->GetTypeID())
    {
        case TypeID_Bool:
        {
            return ::Read<vgBool>(object, member, m_JsonDoc);
        }
        case TypeID_Char:
        {
            if (member->IsConstantArray())
            {
                auto* ptr = reinterpret_cast<char*>(object->GetPtrTo(member));
                if(!ptr || member->GetSize() == 0)
                {
                    return false;
                }

                const auto str = m_JsonDoc[member->GetName()].get<std::string>();
                const auto capacity = member->GetSize() / sizeof(char);
                if(str.size() >= capacity)
                {
                    return false;
                }

                std::copy(str.begin(), str.end(), ptr);
                std::fill(ptr + str.size(), ptr + capacity, '\0');
                return true;
            }
            return ::Read<vgChar>(object, member, m_JsonDoc);
        }
        case TypeID_U32:
        {
            return ::Read<vgU32>(object, member, m_JsonDoc);
        }
        case TypeID_S32:
        {
            return ::Read<vgS32>(object, member, m_JsonDoc);
        }
        case TypeID_U64:
        {
            return ::Read<vgU64>(object, member, m_JsonDoc);
        }
        case TypeID_S64:
        {
            return ::Read<vgS64>(object, member, m_JsonDoc);
        }
        case TypeID_Double:
        {
            return ::Read<vgDouble>(object, member, m_JsonDoc);
        }
        case TypeID_Float:
        {
            return ::Read<vgFloat>(object, member, m_JsonDoc);
        }
        default:
        {
            // Pointers require additional processing
            if(member->IsPointer())
            {
                if(member->IsConstantArray())
                {
                    // TODO: Implement constant array pointers
                    return false;
                }

                // Get the class of the object
                const Class* objectClass = GetClassByID(member->GetTypeID());
                if(!objectClass)
                {
                    return false;
                }

                /*
                 * Create a shared_ptr to the object with a custom deleter.
                 * It's **IMPORTANT** to understand that this design is inherently
                 * unsafe, as the object is not guaranteed to be deleted.
                 * I intend to enforce smart_ptr usage in the future, but for now
                 * this is all I care to do.
                 */
                const auto newObject = ObjectPtr(static_cast<Object*>(objectClass->Create()), [](Object*){});
                if(!newObject)
                {
                    return false;
                }

                // Create a new reader for the object, and deserialize it.
                JsonReader reader(m_JsonDoc[member->GetName()]);
                if(!Object::Deserialize(newObject, reader))
                {
                    return false;
                }

                // Get a pointer to member and set it to the new object,
                // abandon the shared_ptr to the object.
                auto* ptr = reinterpret_cast<Object**>(object->GetPtrTo(member));
                if(ptr != nullptr)
                {
                    *ptr = newObject.get();
                    return true;
                }
                return false;
            }
        }
    }
    return false;
}

const nlohmann::json& vigil::JsonReader::GetJson() const
{
    return m_JsonDoc;
}
