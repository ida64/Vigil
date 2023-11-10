/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_CLASSBUILDER_H
#define VIGILSDK_CLASSBUILDER_H

#include "TypeBuilder.h"

#include <memory>
#include <vector>

#include <range/v3/all.hpp>

#include <Compiler/ClangUtils.h>
#include <Compiler/Specification/BaseSpec.h>
#include <clang-c/Index.h>

#include "FieldBuilder.h"

namespace vigil
{
    class ClassBuilder : public TypeBuilder
    {
    public: // Constructors and Destructor
        /// Default constructor
        ClassBuilder() = default;

        // Default destructor
        ~ClassBuilder() override = default;

        /// Explicit constructor
        /// @param cursor The cursor to build the class from
        /// @note must be a class or struct cursor, exception will be thrown otherwise
        explicit ClassBuilder(CXCursor cursor);

    public: // Methods
        std::string Build() override;

        const CXCursor& GetCursor();

    private: // Private Methods
        void DiscoverChildren();

    private: // Member Variables
        CXCursor m_Cursor;

        std::map<CXCursorKind, std::vector<std::shared_ptr<TypeBuilder>>> m_KindToBuilderMap;

    }; // class ClassBuilder

} // namespace vigil

#endif //VIGILSDK_CLASSBUILDER_H
