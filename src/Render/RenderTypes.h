#pragma once

#ifndef _OPENGL_STUDY_RENDER_REMDERTYPES_
#define _OPENGL_STUDY_RENDER_REMDERTYPES_

#include "glm/glm/vec2.hpp"
#include "glm/glm/vec3.hpp"
#include <string>
#include <iosfwd>

namespace OpenGLStudy
{

namespace Render
{
  
  struct Vertex
  {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
  };

  struct Texture
  {
    enum ETextureType : uint8_t
    {
      Diffuse,
      Specular
    };

    uint32_t ID;
    ETextureType Type;
    std::string Path;

    std::string GetTypeString() const;
  };

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy



#endif // _OPENGL_STUDY_RENDER_REMDERTYPES_