/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_CLANGUTILS_H
#define VIGILSDK_CLANGUTILS_H

#include <clang-c/Index.h>

#include <memory>
#include <tuple>
#include <string>
#include <vector>

#include <absl/status/status.h>
#include <absl/status/statusor.h>

namespace vigil
{
    /// Convert a CXString to a std::string
    /// @note this function automatically dispose of the CXString
    absl::StatusOr<std::string> CXStringToStringSafe(CXString string);

    typedef std::vector<CXCursor> CXCursorVector;

    /// Get all nodes in a translation unit by kind
    /// @param cursor The translation unit
    /// @param kinds The kinds to search for
    /// @return CXCursorVector containing the nodes found
    CXCursorVector GetNodesByKinds(CXCursor cursor, const std::vector<CXCursorKind>& kinds);

    /// Get all annotations on a node
    /// @param cursor The node to get the annotations from
    /// @return std::vector<std::string> containing the annotations
    std::vector<std::string> GetNodeAnnotations(CXCursor cursor);

} // namespace vigil

#endif //VIGILSDK_CLANGUTILS_H
