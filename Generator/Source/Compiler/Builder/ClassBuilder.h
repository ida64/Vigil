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
        /// Build builds the class
        /// @return std::string containing the built class
        std::string Build() override;

        /// GetCursor returns the cursor used to build the class
        /// @return const CXCursor& The cursor
        const CXCursor& GetCursor();

    private: // Private Methods
        /// DiscoverChildren discovers all children of the class
        void DiscoverChildren();

    private: // Member Variables
        /// The cursor used to build the class
        CXCursor m_Cursor;

        /// Map of CXCursorKind to array of TypeBuilder
        std::map<CXCursorKind, std::vector<std::shared_ptr<TypeBuilder>>> m_KindToBuilderMap;

    }; // class ClassBuilder

} // namespace vigil

#endif //VIGILSDK_CLASSBUILDER_H
