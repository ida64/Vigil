/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_FIELDBUILDER_H
#define VIGILSDK_FIELDBUILDER_H

#include "TypeBuilder.h"

#include <range/v3/all.hpp>

#include <absl/strings/string_view.h>
#include <absl/strings/str_format.h>
#include <absl/strings/str_join.h>

#include <clang-c/Index.h>

#include <Compiler/Specification/BaseSpec.h>
#include <Compiler/ClangUtils.h>

namespace vigil
{
    class FieldBuilder : public TypeBuilder
    {
    public:
    public: // Constructors and Destructor
        /// Default constructor
        FieldBuilder() = default;

        // Default destructor
        ~FieldBuilder() override = default;

        /// Explicit constructor
        /// @param cursor The cursor to build the field from
        explicit FieldBuilder(CXCursor cursor);

    public: // Methods
        std::string Build() override;

        const CXCursor& GetCursor();

    protected: // Protected Methods
        void EmitFlagsToArray(std::vector<std::string>& flags);

    private: // Member Variables
        CXCursor m_Cursor;

    }; // class FieldBuilder

} // namespace vigil

#endif //VIGILSDK_FIELDBUILDER_H
