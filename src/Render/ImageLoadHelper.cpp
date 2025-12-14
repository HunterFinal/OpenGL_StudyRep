#include "ImageLoadHelper.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <print>

namespace OpenGLStudy
{

namespace Helper
{
  uint32_t ImageLoadHelper::TextureFromFile(const char* InPath, const std::string& InDirectory)
  {

    std::string fileName{InPath};
    fileName = InDirectory + "/" + fileName;

    // Load image data
    int32_t width, height, nrComponents;
    uint8_t* data = stbi_load(
      fileName.c_str(),
      &width,
      &height,
      &nrComponents,
      0
    );

    // Generate texture object(OpenGL object)
    uint32_t texture{};
    glGenTextures(1, &texture);
  
    // Generate texture
    /**
     * // NOTE GL_TEXTURE_2D means this operation will generate a texture on the currently bound texture object at the same target
     * First argument: Specifies texture target
     * // NOTE For which we want to create a texture for if you want to set eacn mipmap level manually.
     * Second argument: Specifies the mipmap level
     * // NOTE Our image has only RGB values
     * Third argument: Tells OpenGL in what kind of format we want to store the texture
     * Fourth/Fifth argument: Width tand height of the resulting texture
     * // NOTE Some legacy stuff
     * Six argument: Should always be 0
     * // NOTE Loaded image with RGB values and stored them as chars(unsigne byte)
     * Seventh/Eighth argument: Specify the format and datatype of the source image
     * Ninth argument: Actual image data
     */

    if (data != nullptr)
    {
      GLenum format{};
      switch (nrComponents)
      {
        // GL_RED
        case 1:
        {
          format = GL_RED;
        }
        break;

        // GL_RGB
        case 3:
        {
          format = GL_RGB;
        }
        break;

        // GL_RGBA
        case 4:
        {
          format = GL_RGBA;
        }
        break;
      }

      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(data));
      // Call this to generate all the required mipmaps for the currently bound texture automatically
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      
    }
    else
    {
      std::println("Failed to load texture");
    }
    
    // Release image data
    stbi_image_free(data);

    return texture;
  }

  uint32_t ImageLoadHelper::CubemapTextureFromFile(const std::vector<std::string>& InFacesPath)
  {

    uint32_t cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    int32_t width, height, nrChannels;
    for (size_t i = 0; i < InFacesPath.size(); ++i)
    {
      uint8_t* data = stbi_load(InFacesPath[i].c_str(), &width, &height, &nrChannels, 0);

      if (data != nullptr)
      {
        const GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      }
      else
      {
        std::println("Failed to load cubemap texture");
      }

      stbi_image_free(data);

    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return cubemapTexture;
  }

}

}