#include "Mesh.h"
#include "GLSLShader.h"
#include <glad/glad.h>

namespace
{
  GLenum GetTextureIdx(uint32_t offset = 0);
}

namespace OpenGLStudy
{

namespace Render
{
 
  Mesh::Mesh(const std::vector<Vertex>& InVertices, const std::vector<uint32_t>& InIndices, const std::vector<Texture>& InTextures)
    : vertices{InVertices}
    , indices{InIndices}
    , textures{InTextures}
  {
    InitializeMesh();
  }

  Mesh::~Mesh()
  {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  void Mesh::Draw(const GLSLShader& InShader)
  {
    uint32_t diffuseIdx = 1;
    uint32_t specularIdx = 1;

    for (size_t i = 0; i < textures.size(); ++i)
    {
      glActiveTexture(GetTextureIdx(i));

      const Texture& texture = textures[i];
      std::string textureStr{"material."};
      textureStr.append(texture.GetTypeString());

      switch (texture.Type)
      {
        case Texture::Diffuse:
        {
          textureStr.append(std::to_string(diffuseIdx++));
        }
        break;

        case Texture::Specular:
        {
          textureStr.append(std::to_string(specularIdx++));
        }
        break;
      }

      InShader.SetFloat(textureStr.c_str(), static_cast<float>(i));
      glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }

    glActiveTexture(GL_TEXTURE0);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
    glBindVertexArray(0);
  }
  
  void Mesh::InitializeMesh()
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    /**
     * Copy user-defined data into the currently bound buffer
     * First argument: Type of buffer
     * Second argument: Size of the data we want to pass to the buffer
     * Third argument: Actual data we want to send
     * Fourth argument: Specifies how we want the graphics card to manage the given data
     *                  GL_STREAM_DRAW: The data is set only once and used by the GPU at most a few times
     *                  GL_STATIC_DRAW: The data is set only once and used many times
     *                  GL_DYNAMIC_DRAW: The data is changed a lot and used many times
     */
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(decltype(indices)::value_type), indices.data(), GL_STATIC_DRAW);

    // Tell OpenGL how it should interpret vertex data
    /**
     * // NOTE layout (location = 0)
     * First argument: Specifies which vertex attribute we want to configure
     * // NOTE vec3 -> 3
     * Second argument: Size of the vertex attribute.
     * // NOTE vec* in GLSL consists of floating point values -> GL_FLOAT
     * Third argument: Specifies the type of data
     * // NOTE GL_TRUE if we inputting integer data type(int, byte)
     * Fourth argument: Specifies if we want the data to be normalized
     * // NOTE We could've also specified the stride as 0 to let OpenGL determine it.
     * // NOTE Only works when values are tightly packed
     * Fifth argument: Known as the Stride.Space between consecutive vertex attributes
     * // FIXME Explore this in more detail later
     * Sixth argument: Offset of where the position data begins.
     */
    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
  }

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy

namespace
{
  GLenum GetTextureIdx(uint32_t offset)
  {
    return GL_TEXTURE0 + offset;
  }
}