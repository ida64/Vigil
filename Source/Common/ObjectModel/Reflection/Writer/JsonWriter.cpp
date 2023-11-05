/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#include "JsonWriter.h"

#include <iostream>

vgBool vigil::JsonWriter::Write(Object* object, ClassMember* member)
{
    switch(member->GetTypeID())
    {
        case TypeID_Bool:
        case TypeID_Char:
        {
            if (member->IsConstantArray() && member->GetTypeID() == TypeID_Char)
            {
                auto* ptrToString = reinterpret_cast<char*>(object->GetPtrTo(member));

                std::string str(strlen(ptrToString), '\0');
                memcpy_s(str.data(), str.size(), ptrToString, str.size());

                m_JsonDoc[member->GetName()] = str;
                return true;
            }
            m_JsonDoc[member->GetName()] = *reinterpret_cast<bool*>(object->GetPtrTo(member));
            return true;
        }
        case TypeID_U32:
        case TypeID_S32:
        {
            m_JsonDoc[member->GetName()] = *reinterpret_cast<vgU32*>(object->GetPtrTo(member));
            return true;
        }
        case TypeID_U64:
        case TypeID_S64:
        {
            m_JsonDoc[member->GetName()] = *reinterpret_cast<vgU64*>(object->GetPtrTo(member));
            return true;
        }
    }

    return false;
}

const nlohmann::json& vigil::JsonWriter::GetJson() const
{
    return m_JsonDoc;
}
