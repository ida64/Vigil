/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity. 
*/
#ifndef VIGILSDK_COMPILER_H
#define VIGILSDK_COMPILER_H

#include <regex>
#include <numeric>
#include <string_view>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iterator>

#include <utility>

#include <Compiler/ClangUtils.h>
#include <Compiler/Specification/BaseSpec.h>
#include <Compiler/Builder/ClassBuilder.h>

#include <clang-c/Index.h>

#include <plog/Log.h>

namespace vigil
{
    constexpr std::string_view kDefaultCppVersion = "c++20";

    class Compiler
    {
    public: // Constructors and Destructor
        /// Default constructor
        /// @param cppVersion The version of C++ to use
        /// @param path The path to the header file to parse
        Compiler(std::string_view cppVersion, std::filesystem::path path)
                : m_CppVersion(cppVersion)
                , m_Path(std::move(path))
        {
        }

        /// Default destructor
        ~Compiler();

    public: // Methods
        /// Get the version of C++ to use
        /// @return The version of C++ to use
        std::string_view GetCppVersion() const;

        /// Get the path to the header file to parse
        /// @return The path to the header file to parse
        const std::filesystem::path& GetPath() const;

        // Get the contents of the header file
        /// @return The contents of the header file
        const std::string& GetContents() const;

        /// Loads the header file and performs the necessary pre-processing.
        /// @return True if the header was loaded and pre-processed successfully, false otherwise.
        [[nodiscard]] absl::Status Compile();

    protected: // Protected Methods
        /// Pre-process the header file
        /// @return True if the header was pre-processed successfully, false otherwise.
        [[nodiscard]] absl::Status PreProcess();

        /// Initialize Clang and parse the temporary file
        /// @param headerPath The path to the header file
        /// @return True if Clang was initialized and the header was parsed successfully, false otherwise.
        [[nodiscard]] absl::Status ClangInit(const std::string& headerPath);

        /// Add a temp file to the list of files to delete
        /// @param path The path to the temp file
        void AddTempFile(const std::filesystem::path& path);

    private: // Member Variables
        /// The version of C++ to use
        std::string_view m_CppVersion = kDefaultCppVersion;

        /// Local path to the header to parse
        std::filesystem::path m_Path;

        /// The contents of the header file
        std::string m_Contents;

        /// Array of paths to temp files
        /// @note This is used to delete the temp files after parsing
        std::vector<std::filesystem::path> m_TempFiles;

        /// Index of the clang compiler
        CXIndex m_ClangIndex = nullptr;

        /// Translation unit of the clang compiler
        CXTranslationUnit m_ClangTU = nullptr;

        /// Classes that have been specified for reflection
        std::vector<std::shared_ptr<ClassBuilder>> m_SpecifiedClasses;

    }; // class Compiler

} // namespace vigil

#endif //VIGILSDK_COMPILER_H
