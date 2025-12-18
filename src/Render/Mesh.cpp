#include "Mesh.h"
#include "GLSLShader.h"
#include <glad/glad.h>
#include <print>
#include <iostream>

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
    ReleaseMesh();
  }

  void Mesh::Draw(const GLSLShader& InShader, uint32_t InInstancedNum)
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

      InShader.SetInt(textureStr.c_str(), i);
      glBindTexture(GL_TEXTURE_2D, texture.ID);
    }

    // Draw mesh
    glBindVertexArray(VAO);
    /**
     * First argument: Specifies the mode we want to draw in(Similar to glDrawArrays)
     * Second argument: Count or number of elements we'd like to draw
     * Third argument: Type of indices(uint32_t -> GL_UNSIGNED_INT)
     * // NOTE Pass in an index array when you're not using element buffer objects
     * Fourth argument: Specify an offset in the EBO
     * 
     * @see glDrawElements()
     */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    if (InInstancedNum > 1)
    {
      glDrawElementsInstanced(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0, InInstancedNum);
    }
    else
    {
      glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glActiveTexture(GL_TEXTURE0);
  }
  
  void Mesh::InitializeMesh()
  {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glm::vec2 translations[100];
    int32_t index = 0;
    constexpr float offset = 0.1f;
    for (int32_t y = -10; y < 10; y+=2)
    {
      for (int32_t x = -10; x < 10; x+=2)
      {
        const float transX = static_cast<float>(x) / 10.0f + offset;
        const float transY = static_cast<float>(y) / 10.0f + offset;
        translations[index++] = glm::vec2{transX, transY};
      }
    }
    
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glGenBuffers(1, &m_instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(translations), &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(decltype(indices)::value_type), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(translations[0]), reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /**
     * First argument: Vertex attribute
     * Second argument: Attribute divisor.Default is 0
     * Meaning of 0 :                      Update the content of the vertex attribute each iteration of the vertex shader.
     * Meaning of X(X is greater than 0) : Update the content every X instances
     */
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
  }

  void Mesh::ReleaseMesh()
  {
    auto releaseVectorLambda = [](auto& OutVector) static
    {
      OutVector.clear();
      OutVector.shrink_to_fit();
    };

    releaseVectorLambda(vertices);
    releaseVectorLambda(indices);
    releaseVectorLambda(textures);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  Mesh::Mesh(Mesh&& Other) noexcept
    : vertices{std::move(Other.vertices)}
    , indices{std::move(Other.indices)}
    , textures{std::move(Other.textures)}
    , VAO{Other.VAO}
    , VBO{Other.VBO}
    , EBO{Other.EBO}
  {  
    // Set all OpenGL object to non-functional object(0)
    Other.VAO = 0;
    Other.VBO = 0;
    Other.EBO = 0; 
  }
  Mesh& Mesh::operator=(Mesh&& Other) noexcept
  {
    if (this != &Other)
    {
      ReleaseMesh();

      vertices = std::move(Other.vertices);
      indices = std::move(Other.indices);
      textures = std::move(Other.textures);
      VAO = Other.VAO;
      VBO = Other.VBO;
      EBO = Other.EBO;

      // Set all OpenGL object to non-functional object(0)
      Other.VAO = 0;
      Other.VBO = 0;
      Other.EBO = 0;
    }

    return *this;
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