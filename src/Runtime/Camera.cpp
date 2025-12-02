#include "Camera.h"

#include <glm/glm/vec3.hpp>

#include <cmath>

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
  { }

  Camera::~Camera() = default;

  void Camera::UpdateCamera(glm::vec3&& DeltaPosition)
  {
    if (IsNearlyZero(DeltaPosition))
    {
      return;
    }

    m_pos[0] += DeltaPosition.x;
    m_pos[1] += DeltaPosition.y;
    m_pos[2] += DeltaPosition.z;
  }

  glm::vec3 Camera::GetPosition() const
  {
    return glm::vec3{m_pos[0], m_pos[1], m_pos[2]};
  }

  glm::vec3 Camera::GetRotation() const
  {
    return glm::vec3{0.0f, 0.0f, 0.0f};
  }

  glm::vec3 Camera::GetForwardVector() const
  {
    return glm::vec3{0.0f, 0.0f, -1.0f};
  }

  glm::vec3 Camera::GetRightVector() const
  {
    return glm::vec3{0.0f, 0.0f, 0.0f};
  }

  glm::vec3 Camera::GetUpVector() const
  {
    return glm::vec3{0.0f, 1.0f, 0.0f};
  }
}

}