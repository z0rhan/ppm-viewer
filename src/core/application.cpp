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
        std::cerr << "Usage: ppm-viewer <image.ppm>\n";
        return -1;
    }

    std::clog << "Loading: " << m_fileName << '\n';

    Renderer renderer;
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
