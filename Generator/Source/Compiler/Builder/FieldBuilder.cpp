/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/
#include "FieldBuilder.h"

vigil::FieldBuilder::FieldBuilder(CXCursor cursor)
    : m_Cursor(cursor)
{
}

const CXCursor& vigil::FieldBuilder::GetCursor()
{
    return m_Cursor;
}

std::string vigil::FieldBuilder::Build()
{
    constexpr std::string_view kFieldFormat
    = R"({VG_CRC32("%s"), "%s", VG_CRC32("%s"), "%s", offsetof(%s, %s), sizeof(%s), %s})";

    const CXCursor& cursor = this->GetCursor();

    std::string originalType = CXStringToStringSafe(clang_getTypeSpelling(clang_getCursorType(cursor)))
                                    .value_or("invalid_type");
    std::string type = originalType;

    switch(clang_getCursorType(cursor).kind)
    {
        case CXType_ConstantArray:
        {
            // Elaborated element types need extra processing.
            CXType elementType = clang_getElementType(clang_getCursorType(cursor));
            if(elementType.kind == CXType_Elaborated)
            {
                // Extract from the elaborated type the template argument type
                elementType = clang_Type_getTemplateArgumentAsType(
                        elementType,
                        0);

                type = CXStringToStringSafe(clang_getTypeSpelling(elementType))
                            .value_or("invalid_type");
                break;
            }

            type = CXStringToStringSafe(clang_getTypeSpelling(elementType))
                    .value_or(type);
            break;
        }
        case CXType_Elaborated:
        {
            const CXType& templateArgumentType = clang_Type_getTemplateArgumentAsType(
                                                    clang_getCursorType(cursor),
                                                    0);
            type = CXStringToStringSafe(clang_getTypeSpelling(templateArgumentType))
                    .value_or(type);
            break;
        }
        default:
        {
            type = CXStringToStringSafe(clang_getTypeSpelling(clang_getCursorType(cursor)))
                    .value_or(type);
            break;
        }
    }

    const std::string& name = CXStringToStringSafe(clang_getCursorSpelling(cursor))
                                .value_or("invalid_field");

    std::vector<std::string> flags;
    EmitFlagsToArray(flags);

    const std::string& flagsString = absl::StrJoin(flags, "|");

    const CXCursor& parentCursor = clang_getCursorSemanticParent(cursor);
    const std::string& parentName = CXStringToStringSafe(clang_getCursorSpelling(parentCursor))
                                        .value_or("invalid_parent");

    return absl::StrFormat(kFieldFormat.data(),
                            name,
                            name,
                            type,
                            type,
                            parentName,
                            name,
                            originalType,
                            flagsString);
}

void vigil::FieldBuilder::EmitFlagsToArray(std::vector<std::string>& flags)
{
    // Add the default flags.
    flags.emplace_back(parser_specification::kFieldFlag_None);

    const CXCursor& cursor = this->GetCursor();

    // If the field has any annotations then add them to the flags.
    std::vector<std::string> annotations = GetNodeAnnotations(cursor);
    if(!annotations.empty())
    {
        // Remove any annotations that are not within the specification.
        ranges::view::remove_if(annotations, [](const std::string& annotation)
        {
            return kFieldAnnotationToFlagsMap.contains(annotation);
        });

        // Convert the annotations to flags.
        ranges::for_each(annotations, [&flags](const std::string& annotation)
        {
            flags.push_back(kFieldAnnotationToFlagsMap.at(annotation));
        });
    }

    // Certain types require additional flags to be properly serialized.
    switch(clang_getCursorType(cursor).kind)
    {
        case CXTypeKind::CXType_Pointer:
        {
            flags.emplace_back(parser_specification::kFieldFlag_Pointer);
            break;
        }
        case CXTypeKind::CXType_ConstantArray:
        {
            flags.emplace_back(parser_specification::kFieldFlag_ConstantArray);
            break;
        }
        default:
        {
            break;
        }
    }
}
