#include "RenderTypes.h"

#include <cassert>
#include <string>
#include <unordered_map>

namespace OpenGLStudy
{

namespace Render
{
  std::string Texture::GetTypeString() const
  {
    const static std::unordered_map<ETextureType, std::string> sTextureTypeNameTable =
    {
      {Diffuse, std::string{"texture_diffuse"}},
      {Specular, std::string{"texture_specular"}}
    };

    if (!sTextureTypeNameTable.contains(Type))
    {
      // If we stop here, we should add new type to table immediatelly
      assert(false);
      return std::string{"Invalid type"};
    }

    return (*sTextureTypeNameTable.find(Type)).second;
  }

} // namespace OpenGLStudy::Render

} // namespace OpenGLStudy