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
            if(!reader.Read(VG_FROM_PTR_UNSAFE(object), &classMember)
                && classMember.IsRequired())
            {
                return false;
            }
        }
        // Move to the parent class
        currentClass = currentClass->GetParentClass();
    }
    return true;
}