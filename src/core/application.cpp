#include "application.hh"
#include "renderer.hh"

#include <iostream>

Application::Application() {}
Application::~Application() {}

int Application::run(int argc, char** argv)
{
    parseArguments(argc, argv);

    if (m_fileName.empty())
    {
        displayErrorMsg("Usage: ppm-viewer image.ppm");
        return -1;
    }

    if (!loadImageData())
    {
        return -1;
    }

    Renderer renderer(m_imageData);
    renderer.run();

    return 0;
}

void Application::parseArguments(int argc, char** argv)
{
    if (argc == 2)
    {
        m_fileName = argv[1];
    }
}

bool Application::loadImageData()
{
    getImageData(m_fileName, m_imageData);

    if (!m_imageData.isValid())
    {
        displayErrorMsg(m_imageData.exceptionMsg.c_str());
        return false;
    }

    return true;
}

void Application::displayErrorMsg(const char* msg)
{
    if (!tinyfd_messageBox("Error", msg, "Ok", "error", 1))
    {
        std::cerr << "Error: " << msg << '\n';
    }
}
