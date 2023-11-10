/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/

#include "ClangUtils.h"

absl::StatusOr<std::string> vigil::CXStringToStringSafe(CXString string)
{
    const char* result = clang_getCString(string);
    if (!result)
    {
        return absl::InternalError("clang_getCString returned null");
    }

    clang_disposeString(string);

    return result;
}

vigil::CXCursorVector vigil::GetNodesByKinds(CXCursor cursor, const std::vector<CXCursorKind>& kinds)
{
    CXCursorVector nodes;

    struct ClientData
    {
        CXCursorVector* m_Nodes;
        const std::vector<CXCursorKind>& m_Kinds;

    } clientData = {&nodes, kinds};

    clang_visitChildren(cursor,
                        [](CXCursor c, CXCursor parent, CXClientData client_data)
                        {
                            auto* data = reinterpret_cast<ClientData*>(client_data);
                            if (std::find(data->m_Kinds.begin(), data->m_Kinds.end(),
                                          clang_getCursorKind(c)) != data->m_Kinds.end())
                            {
                                data->m_Nodes->push_back(c);
                            }
                            return CXChildVisit_Continue;
                        }, &clientData);

    return nodes;
}

std::vector<std::string> vigil::GetNodeAnnotations(CXCursor cursor)
{
    bool result = false;
    std::vector<std::string> annotations;

    struct ClientData
    {
        std::vector<std::string>* m_Annotations;

    } clientData = {&annotations};

    clang_visitChildren(cursor,
                        [](CXCursor c, CXCursor parent, CXClientData client_data)
                        {
                            if(clang_getCursorKind(c) == CXCursor_AnnotateAttr)
                            {
                                auto* data = reinterpret_cast<ClientData*>(client_data);

                                absl::StatusOr<std::string> spelling
                                        = CXStringToStringSafe(clang_getCursorSpelling(c));

                                if(spelling.ok())
                                {
                                    data->m_Annotations->push_back(std::move(spelling.value()));
                                    return CXChildVisit_Break;
                                }
                            }
                            return CXChildVisit_Recurse;
                        }, &clientData);

    return annotations;
}
