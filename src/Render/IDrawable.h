#pragma once

#ifndef _OPENGL_STUDY_RENDER_IDRAWABLE_
#define _OPENGL_STUDY_RENDER_IDRAWABLE_

namespace OpenGLStudy
{

namespace Render
{

  class IDrawable
  {
    public:
      virtual ~IDrawable() = default;

      virtual void OnDraw() = 0;
  };

}

}

#endif // _OPENGL_STUDY_RENDER_IDRAWABLE_