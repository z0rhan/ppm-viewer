#include "renderer.hh"

#include <iostream>
#include <vector>

const std::string s_shaderPath = "/shader/basic.shader";

Renderer::Renderer(const ImageData& data): m_data(data)
{
    m_imageHeight = m_data.imageHeight;
    m_imageWidth = m_data.imageWidth;
}
Renderer::~Renderer() {}

void Renderer::run()
{
    if (!initGLFW())
    {
        return;
    }

    renderSetup();
    Shader shader(s_shaderPath);
    renderLoop(shader);
}

void Renderer::renderLoop(Shader& shader)
{
    while (!glfwWindowShouldClose(m_window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureId);

        glBindVertexArray(m_VAO);

        shader.bind();
        shader.setUniform1i("imageTexture", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

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

    glViewport(0, 0, m_imageWidth, m_imageHeight);
    glfwSetFramebufferSizeCallback(m_window, updateWindowSize);

    return true;
}

void Renderer::renderSetup()
{
    float vertices[] =
    {
        // Positions  // Textures
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,

         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // Texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    createTexture();
}

void Renderer::createTexture()
{
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    unsigned int maxColorValue = m_data.maxColorValue;

    if (maxColorValue <= 255) {
        // Use 8-bit texture for standard images (max color <= 255)
        std::vector<unsigned char> byteData;
        byteData.reserve(m_data.pixelData.size());
        for (unsigned int value : m_data.pixelData) {
            byteData.push_back(static_cast<unsigned char>(value));
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, byteData.data());

    }
    else
    {
        // Use 16-bit texture for high bit-depth images (max color > 255)
        std::vector<unsigned short> shortData;
        shortData.reserve(m_data.pixelData.size());
        for (unsigned int value : m_data.pixelData) {
            // Scale to 16-bit range (0-65535) based on the actual max color value
            unsigned short scaledValue = static_cast<unsigned short>((value * 65535) / maxColorValue);
            shortData.push_back(scaledValue);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_SHORT, shortData.data());
    }

    glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::updateWindowSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
