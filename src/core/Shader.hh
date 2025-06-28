#ifndef SHADER_HH
#define SHADER_HH

/*
 * This is more of a note for myself
 *
 * Shader class to deal with shaders [Abstractions]
 * - glCreateShader(shadertype[GL_VERTEX_SHADER,...]) -> uint
 * - glShaderSource(unit, &source, nullptr)
 * - glCompileShader(unit)
 * - glDeleteShader(shader)
 *
 * - glCreateProgram() -> uint
 * - glAttachShader(shaderProgram, shader)
 * - glLinkProgram(shaderProgram)
 * The constructor does all the above and stores the shaderProgram as m_renderedId
 *
 * bind() calls glUseProgram(shaderProgram)
 * unbind() calls glUseProgram(0)
 *
 * - glGetUniformLocation(shaderProgram, name)
 * - glUniform4f(uniformLocation, vec4f)
 * setUniform4f() does the above calls to deal with uniforms
 *
 * - glDeleteProgram(shaderProgram)
 * The destructor does the above call
 */


#include <string>
#include <unordered_map>

struct ShaderSource
{
    std::string vertexSource;
    std::string fragmentSource;
};

enum class ShaderType
{
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1,
};

class Shader {
public:
    Shader(const std::string& filePath);
    ~Shader();

    // Use the shader program
    void bind() const;
    // Uses 0 as the shader program
    void unbind() const;

    // I trust ur intelligence
    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);

private:
    unsigned int m_renderedId;
    std::string m_filePath; // For debugging purposes
    std::unordered_map<std::string, int> m_uniformLocationCache; // cache locations

    // reads from source file to get the source for shaders
    static ShaderSource parseShader(const std::string& filePath);    
    // makes and returns a shader program that has vertex and fragment shader
    static unsigned int createShader(const std::string& vertexSource,
                                     const std::string& fragmentSource);
    // GL calls for compiling shader
    static unsigned int compileShader(unsigned int type,const std::string& source);
    // returns the location of an uniform
    int getUniformLocation(const std::string& name);
};


#endif // SHADER_HH
