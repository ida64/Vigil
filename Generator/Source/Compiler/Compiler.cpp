/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/

#include "Compiler.h"

absl::Status vigil::Compiler::Compile()
{
    const std::filesystem::path& path = this->GetPath();
    if(!std::filesystem::exists(path))
    {
        return absl::NotFoundError("The file does not exist");
    }

    // Load the file into memory
    std::ifstream inputFileStream(path);
    if(!inputFileStream.is_open())
    {
        return absl::InternalError("Failed to open file stream");
    }

    // Read the file into a string
    std::string contents((std::istreambuf_iterator<char>(inputFileStream)),
                         std::istreambuf_iterator<char>());

    if(contents.empty())
    {
        return absl::InvalidArgumentError("The contents of the header file are empty");
    }

    m_Contents = std::move(contents);

    if(absl::Status status = this->PreProcess(); !status.ok())
    {
        return status;
    }

    // Build all classes, store output in a vector
    std::vector<std::string> builderResults
    = m_SpecifiedClasses | ranges::views::transform([](const auto& builder)
    {
        return builder->Build();
    }) | ranges::to<std::vector<std::string>>();

    // Join the results into a single string
    std::string result = absl::StrJoin(builderResults, "\n");

    return absl::OkStatus();
}

absl::Status vigil::Compiler::PreProcess()
{
    std::string& contentsRef = m_Contents;
    if (contentsRef.empty())
    {
        return absl::InvalidArgumentError("The contents of the header file are empty");
    }

    // Match all #include statements and insert the typedefs before the first one
    static const std::regex expr(R"(#include\s*["<][^">]*[">])");
    std::smatch match;

    if (!std::regex_search(contentsRef, match, expr) || match.empty())
    {
        return absl::NotFoundError("Failed to find any #include statements");
    }

    contentsRef.insert(match.position(0), parser_specification::kTypedefs);

    // Write the pre-processed header to a temporary file
    // and store it in the list of temporary files to be deleted later
    const std::string kTempFileExtension = ".tmp.hpp";

    const std::filesystem::path& path = this->GetPath();
    std::filesystem::path tempPath = path.parent_path() / (path.filename().string() + kTempFileExtension);

    std::ofstream tempFileStream(tempPath);
    if (!tempFileStream.is_open())
    {
        return absl::InternalError("Failed to open temporary file stream");
    }

    tempFileStream << contentsRef;
    tempFileStream.close();

    // Add the temp file path to the list of temp files to delete
    this->AddTempFile(tempPath);

    // Initialize Clang and parse the temporary file
    if (absl::Status status = this->ClangInit(tempPath.string()); !status.ok())
    {
        return status;
    }

    return absl::OkStatus();
}

absl::Status vigil::Compiler::ClangInit(const std::string& headerPath)
{
    // Create the Clang index
    if(m_ClangIndex = clang_createIndex(0, 0); !m_ClangIndex)
    {
        return absl::InternalError("Failed to create Clang index");
    }

    // Parse the translation unit
    if(m_ClangTU = clang_parseTranslationUnit(m_ClangIndex,
                                              headerPath.c_str(),
                                              nullptr,
                                              0,
                                              nullptr,
                                              0,
                                              CXTranslationUnit_None); !m_ClangTU)
    {
        return absl::InternalError("Failed to parse translation unit");
    }

    const CXTranslationUnit& translationUnit = m_ClangTU;

    auto nodes = GetNodesByKinds(clang_getTranslationUnitCursor(translationUnit),
                                 {CXCursor_ClassDecl, CXCursor_StructDecl});
    for(const CXCursor& node : nodes)
    {
        const std::vector<std::string> annotations = GetNodeAnnotations(node);
        if(!annotations.empty() && std::find(annotations.begin(), annotations.end(),
                                             kAttributes[AttributeIndex_ClassReflection]) != annotations.end())
        {
            PLOG_INFO << (node.kind == CXCursor_ClassDecl ? "Class" : "Struct") << " marked for reflection";

            // Add the class to the list of specified classes
            m_SpecifiedClasses.push_back(std::move(std::make_shared<ClassBuilder>(node)));
        }
    }
    return absl::OkStatus();
}

std::string_view vigil::Compiler::GetCppVersion() const
{
    return m_CppVersion;
}

const std::filesystem::path& vigil::Compiler::GetPath() const
{
    return m_Path;
}

const std::string& vigil::Compiler::GetContents() const
{
    return m_Contents;
}

void vigil::Compiler::AddTempFile(const std::filesystem::path& path)
{
    m_TempFiles.push_back(path);
}

vigil::Compiler::~Compiler()
{
    for(const std::filesystem::path& path : m_TempFiles)
    {
        std::filesystem::remove(path);
    }

    if(m_ClangTU)
    {
        clang_disposeTranslationUnit(m_ClangTU);
    }

    if(m_ClangIndex)
    {
        clang_disposeIndex(m_ClangIndex);
    }
}

