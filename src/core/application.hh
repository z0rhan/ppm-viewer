#ifndef APPLICATION_HH
#define APPLICATION_HH

#include "PPMImage.hh"

#include <string>
#include <tinyfd/tinyfiledialogs.h>

class Application
{
public:
    Application();
    ~Application();

    int run(int argc, char** argv);
private:
    void parseArguments(int argc, char** argv);
    bool loadImageData();
    void displayErrorMsg(const char* msg);

private:
    std::string m_fileName;
    ImageData m_imageData;
};

#endif // APPLICATION_HH
