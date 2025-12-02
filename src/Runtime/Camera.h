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
  
    void UpdateCameraPosition(glm::vec3&& DeltaPosition);
    void UpdateCameraRotation(glm::vec3&& DeltaRotation);
    void UpdateFOV(float DeltaValue);

    glm::vec3 GetPosition() const;
    glm::vec3 GetRotation() const;
    glm::vec3 GetForwardVector() const;
    glm::vec3 GetRightVector() const;
    glm::vec3 GetUpVector() const;
    float GetFOV() const { return m_fov; }
  
  private:
    float m_pos[3];
    float m_rot[3];
    float m_fov;
  };
} // namespace OpenGLStudy::Runtime

} // namespace OpenGLStudy

#endif // _OPENGL_STUDY_CAMERA_