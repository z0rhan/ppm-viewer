#ifndef RENDERER_HH
#define RENDERER_HH

#include "PPMImage.hh"
#include "Shader.hh"

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
    void renderSetup();
    void createTexture();
    void renderLoop(Shader& shader);
    static void updateWindowSize(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* m_window;

    // Hardcoded for now
    // Later, takes the image width and height
    unsigned int m_imageWidth = 800;
    unsigned int m_imageHeight = 600;

    // Image data
    ImageData m_data;

    // Vertex Buffer
    unsigned int m_VAO;
    unsigned int m_VBO;

    // Texture ID
    unsigned int m_textureId;
};

#endif // RENDERER_HH
