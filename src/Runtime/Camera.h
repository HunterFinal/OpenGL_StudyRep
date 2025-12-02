#pragma once

#ifndef _OPENGL_STUDY_CAMERA_
#define _OPENGL_STUDY_CAMERA_

#include <glm/glm/fwd.hpp>

namespace OpenGLStudy
{

namespace Runtime
{
  class Camera final
  {
  
  public:
    Camera();
    ~Camera();
  
    void UpdateCamera(glm::vec3&& DeltaPosition);
  
    glm::vec3 GetPosition() const;
    glm::vec3 GetRotation() const;
    glm::vec3 GetForwardVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetUpVector() const;
  
  private:
    float m_pos[3];
    float m_rot[3];
  };
} // namespace OpenGLStudy::Runtime

} // namespace OpenGLStudy

#endif // _OPENGL_STUDY_CAMERA_