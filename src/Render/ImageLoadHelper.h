#pragma once

#ifndef _OPENGL_STUDY_RENDER_IMAGELOADHELPER_
#define _OPENGL_STUDY_RENDER_IMAGELOADHELPER_

#include <cstdint>
#include <iosfwd>

namespace OpenGLStudy
{

namespace Helper
{

  class ImageLoadHelper
  {
    public:
      static uint32_t TextureFromFile(const char* InPath, const std::string& InDirectory);

    private:
      ImageLoadHelper() = delete;
      ~ImageLoadHelper() = delete;
  };

} // namespace OpenGLStudy::Helper

} // namespace OpenGLStudy


#endif //_OPENGL_STUDY_RENDER_IMAGELOADHELPER_