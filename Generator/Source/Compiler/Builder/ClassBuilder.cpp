/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/

#include "ClassBuilder.h"

#include <plog/Log.h>

vigil::ClassBuilder::ClassBuilder(CXCursor cursor)
    : m_Cursor(cursor), TypeBuilder()
{
    // ClassBuilder can both own a class and a struct.
    if(this->GetCursor().kind != CXCursor_ClassDecl
        && this->GetCursor().kind != CXCursor_StructDecl)
    {
        throw std::runtime_error("ClassBuilder::ClassBuilder: Cursor is not a class or struct!");
    }

    // Discover children nodes (Fields, Enumerations, etc)
    this->DiscoverChildren();
}

std::string vigil::ClassBuilder::Build()
{
    constexpr std::string_view kFieldFormat = R"(
/// Fields: %d
const vigil::ClassMember k%sClassMembers[] =
{
%s
};
)";

    const CXCursor& cursor = this->GetCursor();

    std::vector<std::string> fields;

    for(auto& fieldBuilder : m_KindToBuilderMap[CXCursor_FieldDecl])
    {
        fields.emplace_back(fieldBuilder->Build());
    }

    const std::string& fieldsString = absl::StrJoin(fields, ",\n");

    const std::string& fieldsArray = absl::StrFormat(kFieldFormat.data(),
                                                        fields.size(),
                                                        CXStringToStringSafe(clang_getCursorSpelling(cursor))
                                                            .value_or("invalid_class"),
                                                        fieldsString);
    return fieldsArray;
}

void vigil::ClassBuilder::DiscoverChildren()
{
    const CXCursor& parentCursor = this->GetCursor();

    // Retrieve all public fields and cache them.
    std::vector<CXCursor> cursors = GetNodesByKinds(parentCursor, {CXCursor_FieldDecl, CXCursor_EnumDecl});
    if(!cursors.empty())
    {
        // Remove all non-public declarations.
        ranges::view::remove_if(cursors, [](const CXCursor& cursor)
        {
            return clang_getCXXAccessSpecifier(cursor) != CX_CXXPublic;
        });

        // Insert them into the appropriate vector.
        for(auto& cursor : cursors)
        {
            m_KindToBuilderMap[cursor.kind].push_back(std::move(std::make_shared<FieldBuilder>(cursor)));
        }
    }
}

const CXCursor& vigil::ClassBuilder::GetCursor()
{
    return m_Cursor;
}
