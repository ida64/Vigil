/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/

#include "JsonReader.h"

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

    const nlohmann::json& jsonValue = m_JsonDoc[memberName];

    switch(member->GetTypeID())
    {
        case TypeID_Bool:
        {
            return memcpy_s(object->GetPtrTo(member),
                     member->GetSize(),
                     &jsonValue.get_ref<const vgBool&>(),
                     sizeof(vgBool)) == 0;
        }
        case TypeID_U32:
        {
            return memcpy_s(object->GetPtrTo(member),
                     member->GetSize(),
                     &jsonValue.get_ref<const nlohmann::json::number_unsigned_t&>(),
                     sizeof(vgU32)) == 0;
        }
        case TypeID_S32:
        {
            return memcpy_s(object->GetPtrTo(member),
                     member->GetSize(),
                     &jsonValue.get_ref<const nlohmann::json::number_integer_t&>(),
                     sizeof(vgS32)) == 0;
        }
        case TypeID_U64:
        {
            return memcpy_s(object->GetPtrTo(member),
                     member->GetSize(),
                     &jsonValue.get_ref<const nlohmann::json::number_unsigned_t&>(),
                     sizeof(vgU64)) == 0;
        }
        case TypeID_S64:
        {
            return memcpy_s(object->GetPtrTo(member),
                     member->GetSize(),
                     &jsonValue.get_ref<const nlohmann::json::number_integer_t&>(),
                     sizeof(vgS64)) == 0;
        }
    }

    return true;
}