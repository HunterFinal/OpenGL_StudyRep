#include "Model.h"

#include "ImageLoadHelper.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <print>
#include <algorithm>


namespace OpenGLStudy
{

namespace Render
{

  Model::Model(const std::string& InPath)
    : m_meshes{}
    , m_directory{}
    , m_loadedTextures{}
  {
    LoadModel(InPath);
  }

  void Model::Draw(const GLSLShader& InShader)
  {
    for (Mesh& mesh : m_meshes)
    {
      mesh.Draw(InShader);
    }
  }

  void Model::LoadModel(const std::string& InPath)
  {
    Assimp::Importer importer{};

    /**
     * Second argument: Do extra calculations/operations on the imported data
     * aiProcess_Triangulate: The model does not (entirely) consist of triangles, it should transform all the model's primitive shapes to triangles first
     * aiProcess_FlipUVs: Flips the texture coordinates on the y-axis where necessary during processing(Because we use OpenGL, most images are reversed around the y-axis)
     * aiProcess_GenNormals: Create normal vectors fro each vertex if the model doesn't contain normal vectors
     * aiProcess_SplitLargeMeshes: Splits large meshes into smaller sub-meshes
     * aiProcess_OptimizeMeshes: Does the reverse by trying to join several meshes into one large mesh, reducing drawing calls for optimization
     * 
     * Documentation:
     * URL:https://the-asset-importer-lib-documentation.readthedocs.io/en/latest/usage/postprocessing.html
     */
    const aiPostProcessSteps ppSteps = static_cast<aiPostProcessSteps>(aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    const aiScene* scene = importer.ReadFile(InPath, ppSteps);

    // Check imported model
    if ((scene == nullptr) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (scene->mRootNode == nullptr))
    {
      std::println("{}", importer.GetErrorString());
      return;
    }

    m_directory = InPath.substr(0, InPath.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);

  }

  void Model::ProcessNode(aiNode* Node, const aiScene* InScene)
  {
    if ((Node == nullptr) || (InScene == nullptr))
    {
      return;
    }

    // Process all the node's meshes
    for (uint32_t i = 0; i < Node->mNumMeshes; ++i)
    {
      aiMesh* mesh = InScene->mMeshes[Node->mMeshes[i]];
      if (mesh != nullptr)
      {
        m_meshes.emplace_back(ProcessMesh(mesh, InScene));
      }
    }

    // Do the same thing to its children
    for (uint32_t i = 0; i < Node->mNumChildren; ++i)
    {
      ProcessNode(Node->mChildren[i], InScene);
    }
  }

  Mesh Model::ProcessMesh(aiMesh* _aiMesh, const aiScene* InScene)
  {
    assert(_aiMesh != nullptr);
    assert(InScene != nullptr);

    std::vector<Vertex> vertices{};
    std::vector<uint32_t> indices{};
    std::vector<Texture> textures{};

    vertices.reserve(_aiMesh->mNumVertices);
    // We attempt all face is triangle
    indices.reserve(_aiMesh->mNumFaces * 3);
    textures.reserve(0);

    // Process Vertices
    for (uint32_t i = 0; i < _aiMesh->mNumVertices; ++i)
    {
      Vertex vertex{};

      // Position
      {
        const aiVector3D& aiVec3_Pos = _aiMesh->mVertices[i];
        vertex.Position = glm::vec3{aiVec3_Pos.x, aiVec3_Pos.y, aiVec3_Pos.z};
      }

      // Normal
      {
        if (_aiMesh->HasNormals())
        {
          const aiVector3D& aiVec3_Norm = _aiMesh->mNormals[i];
          vertex.Normal = glm::vec3{aiVec3_Norm.x, aiVec3_Norm.y, aiVec3_Norm.z};
        }
      }

      // Texcoord
      {
        // Assimp allows a model to have up to 8 different texture coordinates per vertex.
        // We only care about the first set of texture coordinates
        if (_aiMesh->mTextureCoords[0] != nullptr)
        {
          const aiVector3D& aiVec3_TexCoord = _aiMesh->mTextureCoords[0][i];
          vertex.TexCoords = glm::vec2{aiVec3_TexCoord.x, aiVec3_TexCoord.y};
        }
        else
        {
          vertex.TexCoords = glm::vec2{0.0f};
        }
      }

      vertices.emplace_back(std::move(vertex));
    }

    // Process indices
    for (uint32_t i = 0; i < _aiMesh->mNumFaces; ++i)
    {
      const aiFace& face = _aiMesh->mFaces[i];
      for (uint32_t j = 0; j < face.mNumIndices; ++j)
      {
        indices.emplace_back(face.mIndices[j]);
      }
    }

    // Process TexCoords
    if (_aiMesh->mMaterialIndex >= 0)
    {
      aiMaterial* mat = InScene->mMaterials[_aiMesh->mMaterialIndex];
      std::vector<Texture> diffuseMap = LoadMaterialTextures(mat, aiTextureType_DIFFUSE, Texture::Diffuse);
      std::copy(diffuseMap.begin(), diffuseMap.end(), std::back_inserter(textures));

      std::vector<Texture> specularMap = LoadMaterialTextures(mat, aiTextureType_SPECULAR, Texture::Specular);
      std::copy(specularMap.begin(), specularMap.end(), std::back_inserter(textures));
    }

    return Mesh{vertices, indices, textures};
  }

  std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* Mat, int32_t Type, Texture::ETextureType InType)
  {
    assert(Mat != nullptr);

    std::vector<Texture> textures{};
    const uint32_t textureCount = Mat->GetTextureCount((aiTextureType)Type);
    textures.reserve(textureCount);
    for (uint32_t i = 0; i < textureCount; ++i)
    {
      aiString str{};
      Mat->GetTexture((aiTextureType)Type, i, &str);
      bool bSkip = false;

      // Find loaded texture
      decltype(m_loadedTextures)::iterator it = std::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), 
        [&str](const Texture& Element)
        {
          return std::strcmp(Element.Path.data(), str.C_Str()) == 0;
        }
      );

      if (it != m_loadedTextures.end())
      {
        textures.emplace_back(*it);
        bSkip = true;
      }

      if (!bSkip)
      {
        Texture texture{};
        texture.ID = OpenGLStudy::Helper::ImageLoadHelper::TextureFromFile(str.C_Str(), m_directory);
        texture.Type = InType;
        texture.Path = str.C_Str();
        textures.emplace_back(texture);
        m_loadedTextures.emplace_back(texture);
      }
    }

    return textures;
  }

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy