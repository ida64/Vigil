/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_JSONREADER_H
#define VIGILSDK_JSONREADER_H

#include <Common/ObjectModel/Object.h>
#include <Common/ObjectModel/Reflection/ObjectReader.h>
#include <nlohmann/json.hpp>

namespace vigil
{
    class JsonReader : public ObjectReader
    {
    public: // Constructors and Destructor
        explicit JsonReader(const nlohmann::json& json);
        virtual ~JsonReader() = default;

    public: // Methods
        vgBool Read(Object* object, ClassMember* member) override;

    private: // Member Variables
        const nlohmann::json& m_JsonDoc;

    }; // class JsonReader

} // namespace vigil

#endif //VIGILSDK_JSONREADER_H
