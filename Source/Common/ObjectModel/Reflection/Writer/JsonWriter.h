/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_JSONWRITER_H
#define VIGILSDK_JSONWRITER_H

#include <Common/ObjectModel/Object.h>
#include <Common/ObjectModel/Reflection/ObjectWriter.h>
#include <Common/Base/BaseDefs.h>

#include <nlohmann/json.hpp>

namespace vigil
{
    class JsonWriter : public ObjectWriter
    {
    public: // Constructors and Destructor
        /// Default constructor
        JsonWriter() = default;

        /// Default destructor
        virtual ~JsonWriter() = default;

    public: // Methods
        /// Write writes the given member from the given object
        /// @param [in] object Pointer to the object to write from
        /// @param [in] member Pointer to the member to write
        /// @return vgBool True if the member was written successfully, false otherwise
        vgBool Write(Object* object, ClassMember* member) override;

        /// GetJson returns the JSON document
        /// @return [out] m_JsonDoc
        const nlohmann::json& GetJson() const;

    private: // Member Variables
        /// JSON document to write to
        nlohmann::json m_JsonDoc;

    }; // class JsonWriter



} // namespace vigil

#endif //VIGILSDK_JSONWRITER_H
