#include "Camera.h"

#include <glm/glm/vec3.hpp>
#include <glm/glm/trigonometric.hpp>
#include <glm/glm/geometric.hpp>

#include <cmath>
#include <algorithm>

namespace
{
  bool IsNearlyZero(const glm::vec3& Vec3)
  {
    constexpr float Tolerance = 0.00001f;
    return    std::abs(Vec3.x) <= Tolerance
           && std::abs(Vec3.y) <= Tolerance
           && std::abs(Vec3.z) <= Tolerance
           ;
  }
}

namespace OpenGLStudy
{

namespace Runtime
{
  Camera::Camera()
    : m_pos{0.0f}
    , m_rot{0.0f}
    , m_fov{45.0f}
  { 
    m_rot[1] = -90.0f;
    m_rot[2] = 0.0f;
  }

  Camera::~Camera() = default;

  void Camera::UpdateCameraPosition(glm::vec3&& DeltaPosition)
  {
    if (!IsNearlyZero(DeltaPosition))
    {
      m_pos[0] += DeltaPosition.x;
      m_pos[1] += DeltaPosition.y;
      m_pos[2] += DeltaPosition.z;
    }
  }

  void Camera::UpdateCameraRotation(glm::vec3&& DeltaRotation)
  {
    if (!IsNearlyZero(DeltaRotation))
    {
      m_rot[0] += DeltaRotation.x;
      m_rot[1] += DeltaRotation.y;
      m_rot[2] += DeltaRotation.z;

      m_rot[0] = std::clamp(m_rot[0], -89.0f, 89.0f);
    }
  }

  void Camera::UpdateFOV(float DeltaValue)
  {
    m_fov = std::clamp(m_fov + DeltaValue, 1.0f, 45.0f);
  }

  glm::vec3 Camera::GetPosition() const
  {
    return glm::vec3{m_pos[0], m_pos[1], m_pos[2]};
  }

  glm::vec3 Camera::GetRotation() const
  {
    return glm::vec3{m_rot[0], m_rot[1], m_rot[2]};
  }

  glm::vec3 Camera::GetForwardVector() const
  {
    const float forwardX = std::cosf(glm::radians(m_rot[1])) * std::cosf(glm::radians(m_rot[0]));
    const float forwardY = std::sinf(glm::radians(m_rot[0]));
    const float forwardZ = std::sinf(glm::radians(m_rot[1])) * std::cosf(glm::radians(m_rot[0]));
    return glm::normalize(glm::vec3{forwardX, forwardY, forwardZ});
  }

  glm::vec3 Camera::GetRightVector() const
  {
    return glm::normalize(glm::cross(GetForwardVector(), glm::vec3{0.0f, 1.0f, 0.0f}));
  }

  glm::vec3 Camera::GetUpVector() const
  {
    return glm::normalize(glm::cross(GetRightVector(), GetForwardVector()));
  }
}

}