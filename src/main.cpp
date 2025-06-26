#include "core/PPMImage.hh"
#include "core/application.hh"

#include <iostream>
#include <string>

static const std::string s_extension = ".ppm";

bool hasPPMextension(std::string& fileName);

int main (int argc, char *argv[]) {
    Application app;

    return app.run(argc, argv);
}

bool hasPPMextension(std::string& fileName)
{
    if (fileName.length() > s_extension.length())
    {
        return fileName.compare(fileName.length() - s_extension.length(), s_extension.length(), s_extension) == 0;
    }

    return false;
}
