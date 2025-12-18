#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec2 aOffset;

// uniform block layout
// Explicitly states the memory layout for each variable type by standardizing their respective offsets

// N = 4bytes
// Scalar : N
// Vector : 2N or 4N (vec3 is 4N)
// Array of Scalar or Vector : Each element has a base alignment equal to that of a vec4(4N)
// Matrices : Stored as a large array of column vectors.Each one has a base alignment of vec4(4N)
// Struct : Equal to the computed size of its elements.Padded to a multiple of the size of a vec4(4N)
layout (std140, binding = 0) uniform Matrices
{
  mat4 projection;
  mat4 view;
};

out VS_OUT
{
  vec3 Normal;
  vec3 FragPos;
  vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 normalMatrix;


void main()
{
  mat4 transform = projection * view * model;

  gl_Position = transform * vec4(aPos + vec3(aOffset, 0.0), 1.0f);
  gl_PointSize = gl_Position.z;

  vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
  vs_out.Normal = mat3(normalMatrix) * aNormal;
  vs_out.TexCoords = aTexCoords;
}