/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_JSONREADER_H
#define VIGILSDK_JSONREADER_H

#include <Common/ObjectModel/Object.h>
#include <Common/ObjectModel/Reflection/ObjectReader.h>

#include <nlohmann/json.hpp>

#include <ios>

namespace vigil
{
    class JsonReader : public ObjectReader
    {
    public: // Constructors and Destructor
        explicit JsonReader(const nlohmann::json& json);
        virtual ~JsonReader() = default;

    public: // Methods
        /// Read reads the given member from the given object
        /// @param [in] object Pointer to the object to read from
        /// @param [in] member Pointer to the member to read
        /// @return vgBool True if the member was read successfully, false otherwise
        vgBool Read(Object* object, ClassMember* member) override;

        /// DocumentToStream writes the JSON document to the given stream
        /// @param [in] stream Stream to write the JSON document to
        void DocumentToStream(std::ostream& stream);

    private: // Member Variables
        /// JSON document to read from
        const nlohmann::json& m_JsonDoc;

    }; // class JsonReader

} // namespace vigil

#endif //VIGILSDK_JSONREADER_H
