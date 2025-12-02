#include "GLSLShader.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <print>
#include <glm/glm/mat4x4.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

namespace OpenGLStudy
{

namespace Render
{
  GLSLShader::GLSLShader(const std::string& vertexPath, const std::string& fragmentPath)
    : m_shaderID{0}
  {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
      // open file
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream shaderStream;

      // read file's buffer 
      shaderStream << vShaderFile.rdbuf();
      vShaderFile.close();
      vertexCode = shaderStream.str();

      std::stringstream{}.swap(shaderStream);

      shaderStream << fShaderFile.rdbuf();
      fShaderFile.close();
      fragmentCode = shaderStream.str();
    }
    catch(const std::ifstream::failure& e)
    {
      std::println(std::cerr, "Error:Shader file read failure.{}", e.what());
    }

    // Initialize shader objects
    constexpr int32_t infoLogLength = 512;
    // Create vertex shader object(OpenGL object)
    uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
    /**
     * First argument: Shader object to compile
     * Second argument: Specifies how many strings we're passing as source code
     * Third argument: Source code of GLSL
     * Fourth argument: We can leave it to NULL //NOTE Figure out why
     */
    {
      const char* vShaderCode = vertexCode.c_str();
      glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
      glCompileShader(vertexShader);
    }

    // Check compilation status
    {
      int32_t success;
      glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        char infoLog[infoLogLength];
        glGetShaderInfoLog(vertexShader, infoLogLength, nullptr, infoLog);
        std::println("ERROR:Shader vertex compilation failed\n{}", infoLog);
      }
    }

    // Create fragment shader object(OpenGL object)
    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    {
      const char* fShaderCode = fragmentCode.c_str();
      glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
      glCompileShader(fragmentShader);
    }

    // Check compilation status
    {
      int32_t success;
      glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        char infoLog[infoLogLength];
        glGetShaderInfoLog(fragmentShader, infoLogLength, nullptr, infoLog);
        std::println("ERROR:Shader fragment compilation failed\n{}", infoLog);
      }
    }

    // Link both vertex and fragment to a shader program object(OpenGL object)
    m_shaderID = glCreateProgram();
    glAttachShader(m_shaderID, vertexShader);
    glAttachShader(m_shaderID, fragmentShader);
    glLinkProgram(m_shaderID);
  
    // Check link status
    {
      int32_t success;
      glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
      if (!success)
      {
        char infoLog[infoLogLength];
        glGetProgramInfoLog(m_shaderID, infoLogLength, nullptr, infoLog);
        std::println("ERROR:Shader program link failed\n{}", infoLog);
      }
    }

    // Delete shader objects if we linked them into the shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
  }

  void GLSLShader::Activate()
  {
    glUseProgram(m_shaderID);
  }

  void GLSLShader::SetBool(const std::string& propertyName, bool value) const
  {
    glUniform1i(glGetUniformLocation(m_shaderID, propertyName.c_str()), static_cast<int32_t>(value));
  }

  void GLSLShader::SetFloat(const std::string& propertyName, float value) const
  {
    glUniform1f(glGetUniformLocation(m_shaderID, propertyName.c_str()), value);
  }

  void GLSLShader::SetInt(const std::string& propertyName, int32_t value) const
  {
    glUniform1i(glGetUniformLocation(m_shaderID, propertyName.c_str()), value);
  }

  void GLSLShader::SetMat4(const std::string& propertyName, glm::mat4 value) const
  {
    glUniformMatrix4fv(glGetUniformLocation(m_shaderID, propertyName.c_str()), 1, GL_FALSE, &value[0][0]);
  }

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy