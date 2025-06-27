#ifndef RENDERER_HH
#define RENDERER_HH

#include "PPMImage.hh"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer
{
public:
    Renderer(const ImageData& data);
    ~Renderer();

    void run();
private:
    bool initGLFW();
    void renderLoop();

private:
    GLFWwindow* m_window;

    // Hardcoded for now
    // Later, takes the image width and height
    unsigned int m_imageWidth = 800;
    unsigned int m_imageHeight = 600;
};

#endif // RENDERER_HH
