#pragma once

#ifndef _OPENGL_STUDY_RENDER_MODEL_
#define _OPENGL_STUDY_RENDER_MODEL_

#include "Mesh.h"
#include <vector>
#include <string>

// Forward declaration
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

enum class EModelRenderMode : uint8_t
{
  Traditional,
  PBR
};

namespace OpenGLStudy
{

namespace Render
{
  // Forward declaration
  class GLSLShader;

  class Model
  {
    public:
      Model(const std::string& InPath, EModelRenderMode InRenderMode = EModelRenderMode::Traditional);

      void Draw(const GLSLShader& InShader);

    private:
      void LoadModel(const std::string& InPath);
      void ProcessNode(aiNode* Node, const aiScene* InScene);
      Mesh ProcessMesh(aiMesh* _aiMesh, const aiScene* InScene);
      std::vector<Texture> LoadMaterialTextures(aiMaterial* Mat, int32_t Type, Texture::ETextureType InType);

    private:
      std::vector<Mesh> m_meshes;
      std::string       m_directory;
      std::vector<Texture> m_loadedTextures;
      EModelRenderMode m_renderMode;
  };

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy


#endif // _OPENGL_STUDY_RENDER_MODEL_