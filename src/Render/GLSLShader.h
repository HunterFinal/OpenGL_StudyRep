#pragma once

#ifndef _OPENGL_STUDY_RENDER_GLSLSHADER_
#define _OPENGL_STUDY_RENDER_GLSLSHADER_

#include <string>
#include <cstdint>

namespace OpenGLStudy
{

namespace Render
{

class GLSLShader
{

public:
  GLSLShader(const std::string& vertexPath, const std::string& fragmentPath);

  void Activate();

  void SetBool(const std::string& propertyName, bool value) const;
  void SetFloat(const std::string& propertyName, float value) const;
  void SetInt(const std::string& propertyName, int32_t value) const;
  uint32_t GetID() const { return m_shaderID; }

private:
  /**ID of shader program */
  uint32_t m_shaderID;
};

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy

#endif // _OPENGL_STUDY_RENDER_GLSLSHADER_