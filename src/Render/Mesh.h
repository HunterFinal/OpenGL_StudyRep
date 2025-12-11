#pragma once

#ifndef _OPENGL_STUDY_RENDER_MESH_
#define _OPENGL_STUDY_RENDER_MESH_

#include "RenderTypes.h"
#include <vector>

namespace OpenGLStudy
{
  namespace Render
  {
    class GLSLShader;
  }
}

namespace OpenGLStudy
{

namespace Render
{
  class Mesh
  {
    public:
      std::vector<Vertex> vertices;
      std::vector<uint32_t> indices;
      std::vector<Texture> textures;

      Mesh(const std::vector<Vertex>& InVertices, const std::vector<uint32_t>& InIndices, const std::vector<Texture>& InTextures);
      ~Mesh() = default;

    public:
      void Draw(const GLSLShader& InShader);

    private:
      void InitializeMesh();

    private:
      uint32_t VAO;
      uint32_t VBO;
      uint32_t EBO;
  };

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy

#endif // _OPENGL_STUDY_RENDER_MESH_