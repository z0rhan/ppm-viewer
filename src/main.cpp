#include "parser.hh"

#include <iostream>
#include <string>

static const std::string s_extension = ".ppm";

bool hasPPMextension(std::string& fileName);

int main (int argc, char *argv[]) {
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " file.ppm\n";
        return EXIT_FAILURE;
    }

    std::string fileName = argv[1];
    if (!hasPPMextension(fileName))
    {
        std::cerr << "Error: .ppm file not found\n";
        return EXIT_FAILURE;
    }

    if (!converToPNG(fileName))
    {
        return EXIT_FAILURE;
    }
    return 0;
}

bool hasPPMextension(std::string& fileName)
{
    if (fileName.length() > s_extension.length())
    {
        return fileName.compare(fileName.length() - s_extension.length(), s_extension.length(), s_extension) == 0;
    }

    return false;
}
