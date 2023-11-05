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
        case TypeID_Char:
        {
            if(member->IsConstantArray() && member->GetTypeID() == TypeID_Char)
            {
                auto str = jsonValue.get_ref<const nlohmann::json::string_t&>();
                if(str.size() <= member->GetSize())
                {
                    return memcpy_s(object->GetPtrTo(member),
                                    member->GetSize(),
                                    str.data(),
                                    str.size()) == 0;
                }
                return false;
            }
            return memcpy_s(object->GetPtrTo(member),
                            member->GetSize(),
                            &jsonValue.get_ref<const vgBool&>(),
                            sizeof(vgChar)) == 0;
        }
        case TypeID_U32:
        case TypeID_S32:
        {
            return memcpy_s(object->GetPtrTo(member),
                            member->GetSize(),
                            &jsonValue.get_ref<const nlohmann::json::number_unsigned_t&>(),
                            sizeof(vgU32)) == 0;
        }
        case TypeID_U64:
        case TypeID_S64:
        {
            return memcpy_s(object->GetPtrTo(member),
                            member->GetSize(),
                            &jsonValue.get_ref<const nlohmann::json::number_unsigned_t&>(),
                            sizeof(vgU64)) == 0;
        }
    }

    return false;
}

void vigil::JsonReader::DocumentToStream(std::ostream& stream)
{
    const std::string& str = m_JsonDoc.dump();
    stream.write(str.data(), str.size());
}
