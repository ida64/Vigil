/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#include "JsonWriter.h"

#include <iostream>

template <typename T>
bool Write(vigil::Object* object, vigil::ClassMember* member, nlohmann::json& m_JsonDoc)
{
    auto* ptr = reinterpret_cast<T*>(object->GetPtrTo(member));
    if(!ptr)
    {
        return false;
    }

    if(member->IsConstantArray())
    {
        std::vector<T> array(ptr, ptr + member->GetSize() / sizeof(T));
        m_JsonDoc[member->GetName()] = array;
        return true;
    }

    m_JsonDoc[member->GetName()] = *ptr;
    return true;
}

template<typename PtrType>
void WriteObjectArray(PtrType* ptr, size_t size, std::vector<nlohmann::json>& elements)
{
    using namespace vigil;

    for(vgS32 idx = 0; idx < size; ++idx)
    {
        PtrType currentObject = ptr[idx];
        if(!currentObject)
        {
            continue;
        }

        JsonWriter writer;
        vgBool result = false;
        if constexpr (std::is_same_v<PtrType, Object*>)
        {
            result = Object::Serialize(ObjectPtr(currentObject, [](Object*) -> void {}), writer);
        }
        else if constexpr (std::is_same_v<PtrType, ObjectPtr>)
        {
            result = Object::Serialize(currentObject, writer);
        }

        if(!result)
        {
            continue;
        }

        elements.push_back(writer.GetJson());
    }
}

vgBool vigil::JsonWriter::Write(Object* object, ClassMember* member)
{
    switch(member->GetTypeID())
    {
        case TypeID_Bool:
        {
            return ::Write<vgBool>(object, member, m_JsonDoc);
        }
        case TypeID_Char:
        {
            if (member->IsConstantArray())
            {
                auto* ptr = reinterpret_cast<char*>(object->GetPtrTo(member));
                m_JsonDoc[member->GetName()] = std::string(ptr);
                return true;
            }
            return ::Write<vgChar>(object, member, m_JsonDoc);
        }
        case TypeID_U32:
        case TypeID_S32:
        {
            return ::Write<vgU32>(object, member, m_JsonDoc);
        }
        case TypeID_U64:
        case TypeID_S64:
        {
            return ::Write<vgU64>(object, member, m_JsonDoc);
        }
        case TypeID_Double:
        {
            return ::Write<vgDouble>(object, member, m_JsonDoc);
        }
        case TypeID_Float:
        {
            return ::Write<vgFloat>(object, member, m_JsonDoc);
        }
        default:
        {
            // Pointers require additional processing
            if(member->IsPointer())
            {
                nlohmann::json& localDocumentRef = m_JsonDoc;

                ObjectPtr objectPtr = nullptr;

                // If the member is a pointer, we need to check if it's a safe (smart pointer) field.
                if(!member->IsSafeField())
                {
                    if(member->IsConstantArray())
                    {
                        auto* ptr = reinterpret_cast<Object**>(object->GetPtrTo(member));
                        if(!ptr)
                        {
                            return false;
                        }

                        std::vector<nlohmann::json> elements;
                        WriteObjectArray<Object*>(ptr, member->GetSize() / sizeof(Object*), elements);

                        localDocumentRef[member->GetName()] = std::move(elements);
                        return true;
                    }

                    auto* ptr = reinterpret_cast<Object**>(object->GetPtrTo(member));
                    if(!ptr)
                    {
                        return false;
                    }

                    // Skip object pointers that haven't been initialized.
                    if(!*ptr)
                    {
                        return false;
                    }

                    // Refer to JsonReader::Read default case for explanation.
                    objectPtr = ObjectPtr(*ptr, [](Object*) -> void {});
                }else
                {
                    if(member->IsConstantArray())
                    {
                        auto* ptr = reinterpret_cast<ObjectPtr*>(object->GetPtrTo(member));
                        if(!ptr)
                        {
                            return false;
                        }

                        std::vector<nlohmann::json> elements;
                        WriteObjectArray<ObjectPtr>(ptr, member->GetSize() / sizeof(ObjectPtr), elements);

                        localDocumentRef[member->GetName()] = std::move(elements);

                        return true;
                    }

                    auto* ptr = reinterpret_cast<ObjectPtr*>(object->GetPtrTo(member));
                    if(!ptr)
                    {
                        return false;
                    }

                    // Skip object pointers that haven't been initialized.
                    if(!*ptr)
                    {
                        return false;
                    }

                    objectPtr = *ptr;
                }

                // Create a new writer for the object, and serialize it.
                JsonWriter writer;
                if(!Object::Serialize(objectPtr, writer))
                {
                    return false;
                }

                // Add the serialized object to the json document.
                localDocumentRef[member->GetName()] = writer.GetJson();

                return true;
            }
        }
    }
    return false;
}

const nlohmann::json& vigil::JsonWriter::GetJson() const
{
    return m_JsonDoc;
}
