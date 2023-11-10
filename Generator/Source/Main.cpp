/*
* (C) 2023 Zel Software, SP
* Please review the license provided before using this project in any capacity.
*/

#include "Main.h"

#include <Compiler/Io/Log.h>
#include <argparse/argparse.hpp>

int main(int argc, char* argv[])
{
    using namespace vigil;

    InitializeLogging();

    argparse::ArgumentParser parser("Vigil Generator");
    parser.add_argument("-h", "--header")
            .help("Path to the header file to parse")
            .default_value(std::string(R"(C:\Users\paging\Desktop\GitHub\VigilSDK\UnitTest\UnitTest.h)"));

    try
    {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err)
    {
        PLOG_ERROR << err.what();
        exit(0);
    }

    std::string path = parser.get<std::string>("--header");

    std::filesystem::path headerPath(path);
    if (!std::filesystem::exists(headerPath))
    {
        PLOG_ERROR << "The specified header file does not exist";
        return -1;
    }

    PLOG_INFO << "Parsing header file: " << headerPath.string();

    const auto& compiler = std::make_shared<Compiler>(kDefaultCppVersion, headerPath);

    absl::Status r = compiler->Compile();
    if (!r.ok())
    {
        PLOG_ERROR << "Failed to compile reflection for header file: " << r.message();
        return -1;
    }

    return 0;
}