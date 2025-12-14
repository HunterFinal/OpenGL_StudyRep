#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
vec3 blur(int idx);

void main()
{
  vec3 col = vec3(0.0);
  for (int i = 0; i < 9; ++i)
  {
    col += blur(i);
  }
  FragColor = vec4(col, 1.0);
}

vec3 blur(int idx)
{
  const float offset = 1.0 / 300.0;

  vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2( 0.0f, offset), // top-center
    vec2( offset, offset), // top-right
    vec2(-offset, 0.0f), // center-left
    vec2( 0.0f, 0.0f), // center-center
    vec2( offset, 0.0f), // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f, -offset), // bottom-center
    vec2( offset, -offset) // bottom-right
    );

  float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
    );

  return texture(screenTexture, TexCoords.st + offsets[idx]).rgb * kernel[idx];
}