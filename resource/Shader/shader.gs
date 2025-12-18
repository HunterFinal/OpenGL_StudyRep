#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
  vec3 Normal;
  vec3 FragPos;
  vec2 TexCoords;
} gs_in[];

out VS_OUT
{
  vec3 Normal;
  vec3 FragPos;
  vec2 TexCoords;
} gs_out;

uniform float time;

vec4 explode(vec4 position, vec3 normal);
vec3 getNormal();

void main()
{
  vec3 normal = getNormal();

  gl_Position = explode(gl_in[0].gl_Position, normal);
  gs_out.Normal = gs_in[0].Normal;
  gs_out.FragPos = gs_in[0].FragPos;
  gs_out.TexCoords = gs_in[0].TexCoords;
  EmitVertex();

  gl_Position = explode(gl_in[1].gl_Position, normal);
  gs_out.Normal = gs_in[1].Normal;
  gs_out.FragPos = gs_in[1].FragPos;
  gs_out.TexCoords = gs_in[1].TexCoords;
  EmitVertex();

  gl_Position = explode(gl_in[2].gl_Position, normal);
  gs_out.Normal = gs_in[2].Normal;
  gs_out.FragPos = gs_in[2].FragPos;
  gs_out.TexCoords = gs_in[2].TexCoords;
  EmitVertex();

  EndPrimitive();
}

vec4 explode(vec4 position, vec3 normal)
{
  float magnitude = 2.0;
  vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
  return position + vec4(direction, 0.0);
}

vec3 getNormal()
{
  vec3 a = gl_in[0].gl_Position.rgb - gl_in[1].gl_Position.rgb;
  vec3 b = gl_in[2].gl_Position.rgb - gl_in[1].gl_Position.rgb;

  return normalize(cross(a, b));
}