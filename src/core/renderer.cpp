#include "renderer.hh"

#include <GLFW/glfw3.h>
#include <iostream>

Renderer::Renderer(const ImageData& data) {}
Renderer::~Renderer() {}

void Renderer::run()
{
    if (!initGLFW())
    {
        return;
    }

    renderLoop();
}

void Renderer::renderLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    // Cleanups
    glfwTerminate();
}

bool Renderer::initGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialized GLFW!!!\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_imageWidth, m_imageHeight, "ppm-viewer", nullptr, nullptr);

    if (!m_window)
    {
        std::cerr << "Failed to create window!!!\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to load GLAD!!!\n";
        glfwTerminate();
        return false;
    }

    return true;
}

