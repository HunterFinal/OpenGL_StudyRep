#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT
{
  vec3 normal;
} vs_out;

layout (std140, binding = 0) uniform Matrices
{
  mat4 projection;
  mat4 view;
};

uniform mat4 model;
uniform mat4 normalMatrix;

void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  mat3 normMat = mat3(normalMatrix);

  vs_out.normal = normalize(vec3(vec4(normMat * aNormal, 1.0)));
}