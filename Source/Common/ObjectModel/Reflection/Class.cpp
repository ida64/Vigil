/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/

#include "Class.h"

vigil::Class* vigil::GetClassByID(vgU32 id)
{
    auto iter = Class::ms_IDToClassMap.find(id);
    if(iter != Class::ms_IDToClassMap.end())
    {
        return iter->second;
    }
    return nullptr;
}