/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/

#include "Object.h"

bool vigil::Object::Deserialize(const ObjectPtr& object, ObjectReader& reader)
{
    Class* currentClass = object->GetClass();
    while(currentClass != nullptr)
    {
        for(auto& classMember : currentClass->GetMembers())
        {
            // Read from the reader with an unsafe pointer to the object,
            // if the member is required and the read fails, return false
            if(!reader.Read(VG_FROM_PTR_UNSAFE(object), classMember)
                && classMember->IsRequired())
            {
                return false;
            }
        }
        // Move to the parent class
        currentClass = currentClass->GetParentClass();
    }
    return true;
}

bool vigil::Object::Serialize(const ObjectPtr& object, ObjectWriter& writer)
{
    Class* currentClass = object->GetClass();
    while (currentClass != nullptr)
    {
        for (auto& classMember: currentClass->GetMembers())
        {
            // Write to the writer with an unsafe pointer to the object,
            // if the member is required and the write fails, return false
            if (!writer.Write(VG_FROM_PTR_UNSAFE(object), classMember)
                && classMember->IsRequired())
            {
                return false;
            }
        }
        // Move to the parent class
        currentClass = currentClass->GetParentClass();
    }
    return true;
}

void* vigil::Object::GetPtrTo(vigil::ClassMember* member)
{
    return reinterpret_cast<vgByte*>(this) + member->GetOffset();
}

