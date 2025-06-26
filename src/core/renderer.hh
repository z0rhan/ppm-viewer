#ifndef RENDERER_HH
#define RENDERER_HH

#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Renderer
{
public:
    Renderer();
    ~Renderer();

    void run();
private:
    bool initGLFW();
    void renderLoop();

private:
    GLFWwindow* m_window;

    unsigned int m_imageWidth = 800;    // Hardcoded for test now
    unsigned int m_imageHeight = 600;
};

#endif // RENDERER_HH
