/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#include "JsonReader.h"

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
        std::vector<T> array = m_JsonDoc[member->GetName()].get<std::vector<T>>();
        std::copy(array.begin(), array.end(), ptr);
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
                auto str = m_JsonDoc[member->GetName()].get<std::string>();
                std::copy(str.begin(), str.end(), reinterpret_cast<char*>(object->GetPtrTo(member)));
                return true;
            }
            return ::Read<vgChar>(object, member, m_JsonDoc);
        }
        case TypeID_U32:
        case TypeID_S32:
        {
            return ::Read<vgU32>(object, member, m_JsonDoc);
        }
        case TypeID_U64:
        case TypeID_S64:
        {
            return ::Read<vgU64>(object, member, m_JsonDoc);
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
                const nlohmann::json& localDocument = this->GetJson();

                // Get the class of the object
                const Class* objectClass = Class::GetClassByID(member->GetTypeID());
                if(!objectClass)
                {
                    return false;
                }

                if(!member->IsSafeField())
                {
                    if(member->IsConstantArray())
                    {
                        auto* ptr = reinterpret_cast<Object**>(object->GetPtrTo(member));

                        const nlohmann::json& array = localDocument[member->GetName()];
                        if(!array.is_array())
                        {
                            return false;
                        }

                        for(vgS32 idx = 0; idx < member->GetSize() / sizeof(Object*); ++idx)
                        {
                            const auto newObject = ObjectPtr(static_cast<Object*>(objectClass->Create()), [](Object*){});
                            if(!newObject)
                            {
                                continue;
                            }

                            const nlohmann::json& jsonObject = array[idx];

                            JsonReader reader(jsonObject);
                            if(!Object::Deserialize(newObject, reader))
                            {
                                continue;
                            }

                            ptr[idx] = newObject.get();
                        }
                        return true;
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
                    JsonReader reader(localDocument[member->GetName()]);
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
                else
                {
                    if(member->IsConstantArray())
                    {
                        auto* ptr = reinterpret_cast<ObjectPtr*>(object->GetPtrTo(member));

                        const nlohmann::json& array = localDocument[member->GetName()];
                        if(!array.is_array())
                        {
                            return false;
                        }

                        for(vgS32 idx = 0; idx < member->GetSize() / sizeof(ObjectPtr); ++idx)
                        {
                            const auto newObject = ObjectPtr(static_cast<Object*>(objectClass->Create()));
                            if(!newObject)
                            {
                                continue;
                            }

                            const nlohmann::json& jsonObject = array[idx];

                            JsonReader reader(jsonObject);
                            if(!Object::Deserialize(newObject, reader))
                            {
                                continue;
                            }

                            ptr[idx] = newObject;
                        }
                        return true;
                    }

                    // Get a pointer to member
                    auto* ptr = reinterpret_cast<std::shared_ptr<Object>*>(object->GetPtrTo(member));
                    if(!ptr)
                    {
                        return false;
                    }

                    // Create a new shared_ptr to the object.
                    const auto newObject = ObjectPtr(static_cast<Object*>(objectClass->Create()));
                    if(!newObject)
                    {
                        return false;
                    }

                    // Create a new reader for the object, and deserialize it.
                    JsonReader reader(localDocument[member->GetName()]);
                    if(!Object::Deserialize(newObject, reader))
                    {
                        return false;
                    }

                    *ptr = newObject;

                    return true;
                }
            }
        }
    }
    return false;
}

const nlohmann::json& vigil::JsonReader::GetJson() const
{
    return m_JsonDoc;
}
