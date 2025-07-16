#include "Shader.hh"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cassert>

//------------------------------------------------------------------------------
// Public
Shader::Shader(const std::string& filePath):
    m_filePath(filePath),
    m_renderedId(0)
{
    ShaderSource source;
    try
    {
        source = parseShader(filePath);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    m_renderedId = createShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_renderedId);
}

void Shader::bind() const
{
    glUseProgram(m_renderedId);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

//------------------------------------------------------------------------------
// Uniform setters
void Shader::setUniform1i(const std::string& name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string& name, float value)
{
    glUniform1f(getUniformLocation(name), value);
}


int Shader::getUniformLocation(const std::string& name)
{
    if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
    {
        return m_uniformLocationCache.at(name);
    }

    int location = glGetUniformLocation(m_renderedId, name.c_str());

    if (location == -1)
    {
        std::cout << "Uniform [" << name << "] does not exist!" << std::endl;
    }
    else
    {
        m_uniformLocationCache[name] = location;
    }

    return location;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Private
ShaderSource Shader::parseShader(const std::string &filePath)
{
    ShaderType currentType = ShaderType::NONE;
    std::stringstream shaderSource[2];

    std::string shaderPath = getBinaryDir().string() + filePath;

    std::ifstream fileObj(shaderPath);
    if (!fileObj)
    {
        throw std::runtime_error("Error reading from file! -> "
                                 + shaderPath);
    }

    std::string line;
    while (std::getline(fileObj, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                currentType = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                currentType = ShaderType::FRAGMENT;
        }
        else
        {
            shaderSource[(int)currentType] << line << "\n";
        }
    }

    return ShaderSource{shaderSource[0].str(), shaderSource[1].str()};
}

unsigned int Shader::createShader(const std::string &vertexSource,
                                  const std::string &fragmentSource)
{
    unsigned int shaderProgram = glCreateProgram();
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

unsigned int Shader::compileShader(unsigned int type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);
    const char* shaderSource = source.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int sucess;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS,  &sucess);
    if (!sucess)
    {
        glGetShaderInfoLog(shader, 512, 0,infoLog);
        std::cerr << "Shader compilation failed:\n";
        std::cerr << infoLog << std::endl;

        assert(sucess); // assert to make sure shader compilation is success
    }

    return shader;
}

fs::path Shader::getBinaryDir()
{
    fs::path binDir;

#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    binDir = std::filesystem::path(buffer);
#else
    binDir = fs::canonical("/proc/self/exe");
#endif

    return binDir.parent_path();
}
